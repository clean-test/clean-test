// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <tuple>

namespace clean_test::utils {

/// Helper to perfectly forward universal references into correct lambda-capturable type.
///
/// A usual usage pattern looks like:
/// @code
/// auto lambda = [fwd = fwd_capture(std::forward<Whatever>(whatever))] mutable {
///     auto & [captured] = fwd;
///     call(std::forward<Whatever>(captured);
/// };
/// @endcode
///
/// @note Using the seemingly simpler [fwd = std::forward<...>(...)]... captures by value and likely is not desirable.
template <typename... Args>
constexpr std::tuple<Args...> fwd_capture(Args &&... args)
{
    return {std::forward<Args>(args)...};
}

}


