// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "Conductor.h"

#include "Badges.h"
#include "CaseEvaluator.h"
#include "ColoringSetup.h"

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
            return true;
        case CaseStatus::fail:
        case CaseStatus::abort:
            return false;
        default:
            std::terminate();
    }
}

class Worker {
public:
    using Next = std::atomic<std::size_t>;

    Worker(Cases & cases, Next & next, ColorTable const & colors) :
        m_cases{cases},
        m_next{next},
        m_evaluator{{std::cout, colors, BufferingMode::testcase}},
        m_results{},
        m_thread{[this] { run(); }}
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

template <typename T>
std::vector<T> & move_append(std::vector<T> & destination, std::vector<T> source)
{
    for (auto & s : source) {
        destination.emplace_back(std::move(s));
    }
    return destination;
}

Results execute_parallel(std::size_t const num_threads, Cases test_cases, ColorTable const & colors)
{
    auto next = std::atomic<std::size_t>{0ul};

    // start workers
    auto workers = std::vector<Worker>{};
    workers.reserve(num_threads);
    while (workers.size() < num_threads) {
        workers.emplace_back(test_cases, next, colors);
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

Conductor::Conductor(ColorTable const & colors, unsigned int const num_jobs) noexcept :
    m_colors(colors), m_num_workers{num_jobs == 0ul ? std::max(1u, std::thread::hardware_concurrency()) : num_jobs}
{}

Conductor::Conductor() noexcept : Conductor{coloring_setup(ColoringMode::automatic), 0ul}
{}

Conductor::Results Conductor::run() const
{
    using Clock = CaseResult::Clock;
    auto const time_start = Clock::now();

    auto & registry = framework::registry();
    std::cout << m_colors.colored(Color::good, badge(BadgeType::title)) << " Running " << registry.size()
              << " test-cases" << std::endl;

    // run cases and collect results
    auto const results = execute_parallel(m_num_workers, std::exchange(registry, {}), m_colors);
    if (not registry.empty()) {
        display_late_registration_warning(registry);
    }

    auto const wall_time = Clock::now() - time_start;
    std::cout << m_colors.colored(Color::good, badge(BadgeType::title)) << " Ran " << results.size() << " test-cases ("
              << utils::WithAdaptiveUnit{wall_time} << " total)" << std::endl;
    report(results);
    return results;
}

void Conductor::report(Results const & results) const
{
    std::size_t num_passed = 0ul;
    for (auto const & result : results) {
        if (passed(result.m_status)) {
            ++num_passed;
        } else {
            std::cout << m_colors.colored(Color::bad, badge(BadgeType::fail)) << ' ' << result.m_name_path << '\n';
        }
    }
    auto const all_have_passed = (num_passed == results.size());
    if (num_passed > 0ul) {
        std::cout << m_colors.colored(Color::good, badge(BadgeType::pass)) << " All "
                  << (all_have_passed ? "" : "other ") << num_passed << " test-cases\n";
    }
    std::cout << std::flush;
}

void Conductor::display_late_registration_warning(std::vector<framework::Case> const & cases) const
{
    std::cout << m_colors[Color::bad] << badge(BadgeType::headline) << " Warning: The following " << cases.size()
              << " test-cases have been registered late:\n";
    for (auto const & tc : cases) {
        std::cout << badge(BadgeType::empty) << "   - " << tc.name().path() << '\n';
    }
    std::cout << badge(BadgeType::headline) << " Registering test-cases dynamically impedes parallel execution."
              << m_colors[Color::off] << std::endl;
}

}
