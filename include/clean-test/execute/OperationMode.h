// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

namespace clean_test::execute {

/// User-configuration for different operation-modes of the entire framework.
enum class OperationMode {
    help, //!< only show explanatory help message
    list, //!< visualize available test-cases (as a tree)
    run, //!< execute test-cases and report results
};

}
