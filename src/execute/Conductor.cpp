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

namespace clean_test::execute {
namespace {

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

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Conductor::Conductor(ColorTable const & colors) noexcept : m_colors(colors)
{}

Conductor::Conductor() noexcept : Conductor{coloring_setup(ColoringMode::automatic)}
{}

Conductor::Results Conductor::run() const
{
    using Clock = CaseResult::Clock;
    auto const time_start = Clock::now();

    auto & registry = framework::registry();
    auto results = std::vector<CaseResult>{};
    std::cout << m_colors.colored(Color::good, badge(BadgeType::title)) << " Running " << registry.size()
              << " test-cases" << std::endl;

    // run cases and collect results
    for (auto & tc : std::exchange(registry, {})) {
        results.emplace_back(CaseEvaluator{m_colors}(tc));
    }
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
