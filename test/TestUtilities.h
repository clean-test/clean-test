// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <clean-test/utils/SourceLocation.h>

#include <iostream>

namespace clean_test::utils {

/// Runtime-version of a @c static_assert for internal testing.
///
/// @note Intentionally avoids name clash with the macro @c assert.
template <typename T>
inline void dynamic_assert(T const & condition, SourceLocation const & where = SourceLocation::current())
{
    if (not static_cast<bool>(condition)) {
        std::cerr << "Failure in " << where.file_name() << ":" << where.line() << std::endl;
        std::terminate();
    }
}

}
