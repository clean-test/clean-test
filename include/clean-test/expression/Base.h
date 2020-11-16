// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <type_traits>
#include <ostream>

namespace clean_test::expression {

class Base {
};

template <typename T>
concept Printable = requires(std::ostream & out, T const & t)
{
    { out << t } -> std::convertible_to<std::ostream &>;
};

template <typename T>
concept IsBase = std::derived_from<std::remove_cvref_t<T>, Base>;

template <typename T>
concept Evaluatable = requires(T const & t) { t.value(); } or requires(T const & t) { static_cast<bool>(t); };

template <typename T>
concept BasicExpression = IsBase<T> and Printable<T> and Evaluatable<T>;

/// Retrieve boolean value of @p t: Convert represented value to @c bool (via @c value iff necessary).
template <BasicExpression T>
constexpr bool evaluate(T const & t)
{
    auto const & value = [&]() constexpr -> decltype(auto)
    {
        if constexpr (requires { t.value(); }) {
            return t.value();
        } else {
            return t;
        }
    }();
    return static_cast<bool>(value);
}

}
