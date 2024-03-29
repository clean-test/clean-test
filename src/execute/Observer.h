// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Observation.h"

#include <utils/Guarded.h>

#include <vector>

namespace clean_test::execute {
class CaseReporter;

/// Thread-safe sink for details about expectation evaluations.
class Observer {
public:
    using Observations = std::vector<Observation>;

    /// Detailed c'tor: Automatically starts the observation aka announces and records timestamp.
    explicit Observer(execute::CaseReporter & reporter) : m_reporter{reporter}
    {}

    /// Register @p observation into m_result.
    void operator()(Observation observation);

    /// Release recorded observations.
    Observations release() &&
    {
        return std::move(m_observations).release();
    }

private:
    execute::CaseReporter & m_reporter; //!< utility for printing details
    utils::Guarded<Observations> m_observations; //!< encountered observations
};

}
