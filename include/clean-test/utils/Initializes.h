// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <type_traits>
#include <concepts>

namespace clean_test::utils {

/// Concept to require parameter @tparam T to serve as valid initializer for @tparam What.
template <typename T, typename What>
concept Initializes = std::is_constructible_v<What, T>;

}
