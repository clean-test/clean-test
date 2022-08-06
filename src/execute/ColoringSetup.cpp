// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "ColoringSetup.h"

#include <exception>

#if _WIN32
#include <io.h>

static bool is_tty()
{
    return _isatty(_fileno(stdout));
}
#else
#include <unistd.h>

static bool is_tty()
{
    return isatty(fileno(stdout));
}
#endif

namespace clean_test::execute {
namespace {

bool are_colors_enabled(ColoringMode const mode)
{
    switch (mode) {
        case ColoringMode::automatic:
            return is_tty();
        case ColoringMode::enabled:
            return true;
        case ColoringMode::disabled:
            return false;
        default:
            std::terminate();
    }
}

constexpr auto colors_disabled = ColorTable{
    "",
    "",
    "",
};

constexpr auto colors_enabled = ColorTable{
    "\x1B[0m", // off
    "\x1B[32m", // green
    "\x1B[31m", // red
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ColorTable const & coloring_setup(ColoringMode const mode)
{
    return are_colors_enabled(mode) ? colors_enabled : colors_disabled;
}

}
