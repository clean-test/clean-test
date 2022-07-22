// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "Conductor.h"

#include "Badges.h"
#include "CaseEvaluator.h"
#include "CaseReporter.h"
#include "ColorTable.h"
#include "ColoringSetup.h"
#include "NameFilter.h"
#include "Observer.h"

#include <framework/FallbackObservationSetup.h>
#include <framework/Registry.h>

#include <utils/OSyncStream.h>
#include <utils/RangesUtils.h>
#include <utils/WithAdaptiveUnit.h>

#include <exception>
#include <iostream>
#include <thread>

namespace clean_test::execute {
namespace {

using Cases = framework::Registry;

bool passed(CaseStatus const status)
{
    switch (status) {
        case CaseStatus::pass:
        case CaseStatus::skip:
            return true;
        case CaseStatus::fail:
        case CaseStatus::abort:
            return false;
        default:
            std::terminate();
    }
}

bool is_regular(CaseResult::Type const & type)
{
    return not static_cast<bool>(type);
}

Conductor::Setup const & default_setup()
{
    static auto const filter = NameFilter{};
    static auto const singleton = Conductor::Setup{
        .m_logger = std::cout,
        .m_colors = coloring_setup(ColoringMode::automatic),
        .m_num_workers = 0u,
        .m_buffering = BufferingMode::testcase,
        .m_filter = filter};
    return singleton;
}

Conductor::Setup normalized(Conductor::Setup const & input)
{
    auto output = input; // intentional copy

    // Map special value of 0 workers to the maximum number supported of concurrently running CPU cores.
    auto & num_jobs = output.m_num_workers;
    if (num_jobs == 0u) {
        num_jobs = std::max(1u, std::thread::hardware_concurrency());
    }

    return output;
}

template <typename T>
std::vector<T> & move_append(std::vector<T> & destination, std::vector<T> source)
{
    for (auto & s : source) {
        destination.emplace_back(std::move(s));
    }
    return destination;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Worker {
public:
    using Next = std::atomic<std::size_t>;

    Worker(Cases & cases, Next & next, Conductor::Setup setup) :
        m_cases{cases},
        m_next{next},
        m_filter{std::move(setup.m_filter)},
        m_evaluator{
            {.m_output = setup.m_logger,
             .m_colors = std::move(setup.m_colors),
             .m_buffering = std::move(setup.m_buffering)},
            setup.m_num_workers == 1,
        },
        m_results{},
        m_thread{[this] { run(); }}
    {}

    void join()
    {
        m_thread.join();
    }

    auto results() &&
    {
        return std::move(m_results);
    }

private:
    void run()
    {
        auto const num = m_cases.size();

        auto next = [previous = std::size_t{0}, this, num]() mutable {
            while (previous < num
                   and not m_next.compare_exchange_weak(previous, previous + 1, std::memory_order_relaxed)) {
            }
            return previous;
        };

        for (auto cur = next(); cur < num; cur = next()) {
            m_results.emplace_back(evaluate(m_cases[cur]));
        }
    }

    CaseResult evaluate(framework::Case & tc)
    {
        switch (m_filter(tc.name())) {
            case NameFilterToggle::enabled:
                // Execute test-case.
                return m_evaluator(tc);

            case NameFilterToggle::disabled:
                // Skip test-case.
                return CaseResult{std::string{tc.name().path()}, CaseStatus::skip, CaseResult::Duration{}, {}};

            default:
                std::terminate();
        }
    }

    Cases & m_cases;
    Next & m_next;
    NameFilter const & m_filter;
    CaseEvaluator m_evaluator;
    Outcome::Results m_results;
    std::thread m_thread;
};

auto execute_parallel(Cases test_cases, Conductor::Setup const setup)
{
    auto const num_threads = setup.m_num_workers;
    auto next = std::atomic<std::size_t>{0ul};

    // start workers
    auto workers = std::vector<Worker>{};
    workers.reserve(num_threads);
    while (workers.size() < num_threads) {
        workers.emplace_back(test_cases, next, setup);
    }

    // collect results
    auto results = Outcome::Results{};
    for (auto & worker : workers) {
        worker.join();
        move_append(results, std::move(worker).results());
    }
    return results;
}

/// Variant of @c execute_parallel that manages mis-reported @c Observation s encountered at a fallback observer.
Outcome safe_execute_parallel(Cases test_cases, Conductor::Setup const setup)
{
    using Clock = CaseResult::Clock;
    auto const time_start = Clock::now();

    // Ensure appropriate fallback observation setup.
    auto fallback_reporter = CaseReporter{{setup.m_logger, setup.m_colors, setup.m_buffering}};
    auto fallback_observer = Observer{fallback_reporter};
    auto const fallback = framework::FallbackObservationSetup{fallback_observer};

    // Run all test-cases in parallel (with the ensured fallback observation setup).
    auto results = execute_parallel(std::move(test_cases), setup);

    // Harvest any incorrectly directed observations.
    auto unmanaged = std::move(fallback_observer).release();
    if (not unmanaged.empty()) {
        auto const & colors = setup.m_colors;
        utils::OSyncStream{setup.m_logger}
            << colors[Color::bad] << badge(BadgeType::headline)
            << " Warning: Observed test-expectations at unknown Observer, likely caused by lacking passed observer."
            << colors[Color::off] << std::endl;
        results.emplace_back(
            "unknown", CaseStatus::pass, CaseResult::Duration{}, std::move(unmanaged), CaseResult::Type::fallback);
    }
    return {Clock::now() - time_start, std::move(results)};
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Conductor::Conductor(Setup const & setup) noexcept : m_setup{normalized(setup)}
{}

Conductor::Conductor() noexcept : Conductor{default_setup()}
{}

Outcome Conductor::run() const
{
    auto & registry = framework::registry();
    m_setup.m_logger
        << m_setup.m_colors.colored(Color::good, badge(BadgeType::title)) << " Running " << registry.size()
        << " test-cases" << std::endl;

    // run cases and collect results
    auto const outcome = safe_execute_parallel(std::exchange(registry, {}), m_setup);
    if (not registry.empty()) {
        display_late_registration_warning(registry);
    }

    report(outcome);
    return outcome;
}

void Conductor::report(Outcome const & outcome) const
{
    auto const & [wall_time, results] = outcome;
    auto const & colors = m_setup.m_colors;
    auto const num_regular_tests = utils::count_if(results, [](CaseResult const & r) { return is_regular(r.m_type); });

    auto & logger = m_setup.m_logger;
    logger
        << colors.colored(Color::good, badge(BadgeType::title)) << " Ran " << num_regular_tests << " test-cases ("
        << utils::WithAdaptiveUnit{wall_time} << " total)" << std::endl;

    std::size_t num_passed = 0ul;
    for (auto const & result : results) {
        if (passed(result.m_status)) {
            num_passed += is_regular(result.m_type);
        } else {
            logger << colors.colored(Color::bad, badge(BadgeType::fail)) << ' ' << result.m_name_path << '\n';
        }
    }
    auto const all_have_passed = (num_passed == num_regular_tests);
    if (num_passed > 0ul) {
        logger
            << colors.colored(Color::good, badge(BadgeType::pass)) << " All " << (all_have_passed ? "" : "other ")
            << num_passed << " test-cases\n";
    }
    logger << std::flush;
}

void Conductor::display_late_registration_warning(std::vector<framework::Case> const & cases) const
{
    auto const & colors = m_setup.m_colors;
    auto & logger = m_setup.m_logger;

    logger
        << colors[Color::bad] << badge(BadgeType::headline) << " Warning: The following " << cases.size()
        << " test-cases have been registered late:\n";
    for (auto const & tc : cases) {
        logger << badge(BadgeType::empty) << "   - " << tc.name().path() << '\n';
    }
    logger
        << badge(BadgeType::headline) << " Registering test-cases dynamically impedes parallel execution."
        << colors[Color::off] << std::endl;
}

}
