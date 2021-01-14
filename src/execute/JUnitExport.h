// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"

#include <iosfwd>
#include <vector>

namespace clean_test::execute {

/// XML serializable wrapper about test-execution results.
class JUnitExport {
public:
    using Results = std::vector<CaseResult>;
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;

    /// Write XML representation of @p data into @p out.
    friend std::ostream & operator<<(std::ostream & out, JUnitExport data);

    Results const & m_results; //!< results of all executed test-cases.
    Duration m_wall_time; //!< elapsed wall-time of entire test-case execution.
};

}
