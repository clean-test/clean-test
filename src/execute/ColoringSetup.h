// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ColorTable.h"

#include <execute/ColoringMode.h>

namespace clean_test::execute {

/// Access coloring details for @p mode.
ColorTable const & coloring_setup(ColoringMode mode);

}
