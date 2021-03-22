// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <type_traits>
#include <concepts>

namespace clean_test::expression {

class Base {
};

template <typename T>
concept IsBase = std::derived_from<std::remove_cvref_t<T>, Base>;

template <typename T>
concept BasicExpression = IsBase<T>;

}
