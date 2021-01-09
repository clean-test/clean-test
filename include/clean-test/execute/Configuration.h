// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "BufferingMode.h"
#include "ColoringMode.h"

namespace clean_test::execute {

/// User settings for running tests.
class Configuration {
public:
    /// Parse @c Configuration from commandline arguments @p argc and @p argv.
    static Configuration parse(int argc, char ** argv);

    bool print_help = false;
    bool list_tests = false;
    bool run_tests = true;

    /// How console output of the framework should be colored.
    ColoringMode coloring = ColoringMode::automatic;
    /// Whether test-case messages should be buffered (in order to ensure uninterrupted output).
    BufferingMode buffering = BufferingMode::testcase;

    /// Number of test-cases to be executed in parallel.
    ///
    /// The special value 0 (default) instructs to utilize all available CPU cores.
    unsigned int num_jobs = 0ul;
};

}
