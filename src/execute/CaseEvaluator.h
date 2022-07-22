// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"
#include "CaseReporter.h"

#include <framework/Case.h>

namespace clean_test::execute {

/// Assesses testcase execution and thereby collects results.
class CaseEvaluator {
public:
    /// Detailed c'tor: Build reporter from given @p setup.
    explicit CaseEvaluator(CaseReporter::Setup setup, bool overwrite_fallback_observer);

    /// Execute @p testcase and return results evaluated therein.
    CaseResult operator()(framework::Case & testcase) noexcept;

private:
    CaseReporter m_reporter; //!< output facility for test status and observations.
    /// Control whether exchanging the global fallback observer by the test-case specific observer is desired. This is
    /// only done temporarily while a testcase is being evaluated and reverted afterwards.
    /// This is intended for the sequential mode where the case-specific observer should also be available globally.
    bool const m_overwrite_fallback_observer;
};

}
