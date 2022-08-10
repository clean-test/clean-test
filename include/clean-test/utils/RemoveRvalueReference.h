// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <type_traits>

namespace clean_test::utils {

/// Removes rvalue references from T, but preserves lvalue references.
template <typename T>
using RemoveRvalueReference = std::conditional_t<std::is_rvalue_reference_v<T>, std::remove_reference_t<T>, T>;

}
