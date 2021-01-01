// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

namespace clean_test::execute {

/// High level configuration for the coloring of the printed commandline output.
enum class ColoringMode {
    automatic = 0, //!< enable colors iff supported by the receiver (terminal?)
    enabled = 1, //!< use colors (unconditionally)
    disabled = 2, //!< don't use any colors whatsoever
};

}
