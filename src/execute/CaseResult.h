// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseStatus.h"
#include "Observation.h"

#include <chrono>
#include <cstdint>
#include <vector>

namespace clean_test::execute {

/// Collection of observation details about executing a single test-case.
class CaseResult {
public:
    using Clock = std::chrono::system_clock;
    using Duration = std::chrono::nanoseconds;
    using TimePoint = std::chrono::time_point<Clock, Duration>;
    using Observations = std::vector<Observation>;

    /// Detailed c'tor: initialize from @p name_path, @p wall_time and @p observations.
    ///
    /// Stores worst outcome of any @p observations and @p execution_outcome into @c m_outcome.
    /// The @p execution_outcome is assumed to result from the test execution.
    CaseResult(std::string name_path, CaseStatus execution_outcome, Duration wall_time, Observations observations);

    std::string m_name_path; //!< Name of the test-case.
    CaseStatus m_status; //!< Overall outcome of the test-case.
    Duration m_wall_time; //!< Total execution (wall) time.
    Observations m_observations; //!< Observation details (including passed ones).
};

}
