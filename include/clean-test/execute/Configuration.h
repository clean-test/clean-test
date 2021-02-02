// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "BufferingMode.h"
#include "ColoringMode.h"
#include "NameFilterSetting.h"
#include "OperationMode.h"

#include <vector>
#include <filesystem>

namespace clean_test::execute {

/// User settings for running tests.
class Configuration {
public:
    /// Parse @c Configuration from commandline arguments @p argc and @p argv.
    ///
    /// Throws std::invalid_argument (with descriptive error) in case parsing the given commandline is not possible.
    static Configuration parse(int argc, char const * const * argv);

    /// @name General Configuration
    ///
    /// @{

    /// Which mode of clean-test should be executed?
    OperationMode m_operation = OperationMode::run;

    /// How console output of the framework should be colored.
    ColoringMode m_coloring = ColoringMode::automatic;

    /// Which selection of test-cases should be included / excluded from configured operations?
    std::vector<NameFilterSetting> m_filter_settings = {};

    /// @}

    /// @name Test Execution Configuration
    ///
    /// @{

    /// Whether test-case messages should be buffered (in order to ensure uninterrupted output).
    BufferingMode m_buffering = BufferingMode::off;

    /// Number of test-cases to be executed in parallel.
    ///
    /// The special value 0 (default) instructs to utilize all available CPU cores.
    unsigned int m_num_jobs = 0ul;

    /// Path for generating the JUnit-XML summary into.
    ///
    /// The special value of @c {} (empty, default) disables the generation of this type of report.
    std::filesystem::path m_junit_path = {};

    /// @}

    /// @name Test Listing Configuration
    ///
    /// @{

    /// Maximum depth of the visualized tree of test-cases.
    unsigned int m_depth = 3ul;

    /// @}
};

}
