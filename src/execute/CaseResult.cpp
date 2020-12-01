// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "CaseResult.h"

#include <algorithm>
#include <concepts>
#include <version>

namespace clean_test::execute {
namespace {

/// Variant of @c std::any_of for whole containers.
template <typename Data, std::invocable<typename Data::value_type> Predicate>
constexpr bool any_of(Data const & data, Predicate && predicate)
{
#if __cpp_lib_ranges
    return std::ranges::any_of(data, std::forward<Predicate>(predicate));
#else
    return std::any_of(std::cbegin(data), std::cend(data), std::forward<Predicate>(predicate));
#endif
}

/// Convert most severe failure reason of any @p observations to @c CaseOutcome.
CaseStatus worst_status(CaseResult::Observations const & observations)
{
    auto const has_failed = any_of(observations, [](auto const & observation) {
        return observation.m_status == ObservationStatus::fail;
    });
    return has_failed ? CaseStatus::fail : CaseStatus::pass;
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CaseResult::CaseResult(
    std::string name_path,
    CaseStatus execution_outcome,
    Duration wall_time,
    Observations observations) :
    m_name_path{name_path},
    m_status{std::max(execution_outcome, worst_status(observations))},
    m_wall_time{std::move(wall_time)},
    m_observations{std::move(observations)}
{}

}
