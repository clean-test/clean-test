// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ObservationStatus.h"

#include <clean-test/utils/SourceLocation.h>

#include <string>

namespace clean_test::execute {

/// Observation details e.g. about a tested expectation.
class Observation {
public:
    utils::SourceLocation m_where; //!< Location where observation was conducted.
    ObservationStatus m_status; //!< Outcome of the observation.
    std::string m_expression_details; //!< Evaluation details leading to given state.
    std::string m_description; //!< User-added message piped into the expectation.
};

}

