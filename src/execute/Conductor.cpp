// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "Conductor.h"

#include "Badges.h"
#include "CaseEvaluator.h"
#include "ColorTable.h"
#include "ColoringSetup.h"
#include "NameFilter.h"

#include <framework/Registry.h>

#include <utils/WithAdaptiveUnit.h>

#include <exception>
#include <iostream>
#include <thread>

namespace clean_test::execute {
namespace {

using Cases = framework::Registry;
using Results = Conductor::Results;

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

Conductor::Setup const & default_setup()
{
    static auto const filter = NameFilter{};
    static auto const singleton = Conductor::Setup{
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

    Worker(Cases & cases, Next & next, CaseReporter::Setup const setup) :
        m_cases{cases}, m_next{next}, m_evaluator{setup}, m_results{}, m_thread{[this] { run(); }}
    {}

    void join()
    {
        m_thread.join();
    }

    Results results() &&
    {
        return std::move(m_results);
    }

private:
    void run()
    {
        auto const num = m_cases.size();

        auto next = [previous = 0ul, this, num]() mutable {
            while (previous < num
                   and not m_next.compare_exchange_weak(previous, previous + 1ul, std::memory_order_relaxed)) {
            }
            return previous;
        };

        for (auto cur = next(); cur < num; cur = next()) {
            m_results.emplace_back(m_evaluator(m_cases[cur]));
        }
    }

    Cases & m_cases;
    Next & m_next;
    CaseEvaluator m_evaluator;
    Results m_results;
    std::thread m_thread;
};

Results execute_parallel(std::size_t const num_threads, Cases test_cases, CaseReporter::Setup const setup)
{
    auto next = std::atomic<std::size_t>{0ul};

    // start workers
    auto workers = std::vector<Worker>{};
    workers.reserve(num_threads);
    while (workers.size() < num_threads) {
        workers.emplace_back(test_cases, next, setup);
    }

    // collect results
    auto results = Conductor::Results{};
    for (auto & worker : workers) {
        worker.join();
        move_append(results, std::move(worker).results());
    }
    return results;
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Conductor::Conductor(Setup const & setup) noexcept : m_setup{normalized(setup)}
{}

Conductor::Conductor() noexcept : Conductor{default_setup()}
{}

Conductor::Results Conductor::run() const
{
    using Clock = CaseResult::Clock;
    auto const time_start = Clock::now();

    auto & registry = framework::registry();
    auto const & [colors, num_workers, buffering, filter] = m_setup;
    std::cout << colors.colored(Color::good, badge(BadgeType::title)) << " Running " << registry.size()
              << " test-cases" << std::endl;

    // run cases and collect results
    auto const report_setup = CaseReporter::Setup{
        .m_output = std::cout, .m_colors = colors, .m_buffering = buffering};
    auto const results = execute_parallel(num_workers, std::exchange(registry, {}), report_setup);
    if (not registry.empty()) {
        display_late_registration_warning(registry);
    }

    auto const wall_time = Clock::now() - time_start;
    std::cout << colors.colored(Color::good, badge(BadgeType::title)) << " Ran " << results.size() << " test-cases ("
              << utils::WithAdaptiveUnit{wall_time} << " total)" << std::endl;
    report(results);
    return results;
}

void Conductor::report(Results const & results) const
{
    auto const & colors = m_setup.m_colors;
    std::size_t num_passed = 0ul;
    for (auto const & result : results) {
        if (passed(result.m_status)) {
            ++num_passed;
        } else {
            std::cout << colors.colored(Color::bad, badge(BadgeType::fail)) << ' ' << result.m_name_path << '\n';
        }
    }
    auto const all_have_passed = (num_passed == results.size());
    if (num_passed > 0ul) {
        std::cout << colors.colored(Color::good, badge(BadgeType::pass)) << " All "
                  << (all_have_passed ? "" : "other ") << num_passed << " test-cases\n";
    }
    std::cout << std::flush;
}

void Conductor::display_late_registration_warning(std::vector<framework::Case> const & cases) const
{
    auto const & colors = m_setup.m_colors;
    std::cout << colors[Color::bad] << badge(BadgeType::headline) << " Warning: The following " << cases.size()
              << " test-cases have been registered late:\n";
    for (auto const & tc : cases) {
        std::cout << badge(BadgeType::empty) << "   - " << tc.name().path() << '\n';
    }
    std::cout << badge(BadgeType::headline) << " Registering test-cases dynamically impedes parallel execution."
              << colors[Color::off] << std::endl;
}

}
