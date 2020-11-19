// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

namespace clean_test::execute {

/// User settings for running tests.
class Configuration {
public:
    /// Parse @c Configuration from commandline arguments @p argc and @p argv.
    static Configuration parse(int argc, char ** argv);

    bool print_help = false;
    bool list_tests = false;
    bool run_tests = true;
};

}
