// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"
#include "CaseReporter.h"

#include <framework/Case.h>

namespace clean_test::execute {

class ColorTable;

/// Assesses testcase execution and thereby collects results.
class CaseEvaluator {
public:
    /// Detailed c'tor: Build reporter from @p colors.
    CaseEvaluator(ColorTable const & colors);

    /// Execute @p testcase and return results evaluated therein.
    CaseResult operator()(framework::Case& testcase) noexcept;

private:
    CaseReporter m_reporter; //!< output facility for test status and observations.
};

}
