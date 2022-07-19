// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "HelpDisplay.h"

#include <execute/Configuration.h>

namespace clean_test::execute {


std::ostream & operator<<(std::ostream & out, HelpDisplay const & hd)
{
    static auto const default_config = Configuration{};
    auto const & [colors] = hd;
    auto c = [&colors = colors](std::string_view const t) { return colors.colored(Color::good, t); };

    return out
        << "This program contains unit-tests written using Clean Test.\n"
           "It's behavior can be controlled with the following command line flags:\n"
           "\n"
           "Operation mode:\n"
           "By default test-cases are executed. Mutually exclusive alternatives are:\n"
           "  " << c("--help") << "  " << c("-h") << "\n"
           "    Display this help and exit.\n"
           "  " << c("--list") << "  " << c("-l") << "\n"
           "    Instead of executing a test, display its name. This mode honors all general\n"
           "    options.\n"
           "\n"
           "General options:\n"
           "  " << c("--color") << "=(" << c("automatic") << '|' << c("always") << '|' << c("never") << ")\n"
           "    Control whether the output of the framework is colored (default: " << c("automatic") << ").\n"
           "  " << c("--filter")
        << "=[" << c("+-") << "]?(" << c("path") << ":|" << c("tag") << ":|" << c("any") << ":)?PATTERN\n"
           "    Select test-cases to operate on (default: all test-cases). With a leading \n"
           "    " << c("+") << '/' << c("-") << " tests can be in- / excluded (default: " << c("+") << ")."
           " A PATTERN can be applied to \n    either the " << c("name") << ", a " << c("tag")
        << " or " << c("any") << " of the two (default: " << c("any") << "). Multiple filters\n"
           "    are supported; the first match decides.\n"
           "    Example: \"-tag:a.*b\" disables test-cases tagged with an 'a' before a 'b'.\n"
           "\n"
           "Execution options:\n"
           "  " << c("--buffered") << "  " << c("-b") << "\n"
           "    Enable buffering of messages for each test-case.\n"
           "  " << c("--jobs") << "=N  " << c("-j") << " N\n"
           "    Execute at most N test-cases in parallel (default: " << default_config.m_num_jobs << "). "
           "The special value 0 \n"
           "    instructs to utilize all available CPU cores.\n"
           "  " << c("--report") << "=(" << c("junit") << ":)?PATH\n"
           "    Generate output in PATH with specified format (default: " << c("junit") << "-xml).\n"
           "\n"
           "Listing options:\n"
           "  " << c("--depth") << "=N  " << c("-d") << " N\n"
           "    Configure maximum depth N of the visualized tree (default: " << default_config.m_depth << ").\n"
           "\n"
           "For more information, please read the Clean Test documentation available at\n"
        << c("https://github.com/clean-test/clean-test") << ". If you happen to find a bug in \n"
           "Clean Test, you are welcome to open an issue or pull request there.\n";
}

}
