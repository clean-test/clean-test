// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"

#include <chrono>
#include <vector>

namespace clean_test::execute {

/// Overall details collected from a conducted test-cases evaluation.
class Outcome {
public:
    using Results = std::vector<CaseResult>;
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;

    Duration m_wall_time; //!< elapsed wall-time of entire test-case execution.
    Results m_results; //!< test-case specific results of all executed tests.
};

}
