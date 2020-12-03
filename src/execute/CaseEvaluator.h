// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"

#include "framework/Case.h"

namespace clean_test::execute {

/// Assesses testcase execution and thereby collects results.
class CaseEvaluator {
public:
    /// Execute @p testcase and return results evaluated therein.
    CaseResult operator()(framework::Case& testcase) noexcept;
};

}
