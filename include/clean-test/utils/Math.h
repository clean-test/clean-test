// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cmath>

namespace clean_test::utils {
namespace math_details {

/// Checks that T has a @c std::abs from the standard library.
///
/// @note By specification of the standard library, any type with abs-support is always signed.
template <typename T>
concept HasStdAbsSupport = requires {
    { std::abs(std::remove_cvref_t<T>{}) } -> std::same_as<std::remove_cvref_t<T>>;
};

/// Ensures that a manually implemented version of norm could work (e.g. for float).
template <typename T>
concept ManuallyNormable = requires(T t) {
   requires(std::is_constructible_v<std::remove_cvref_t<T>>); // default constructible for "0".
   requires(std::totally_ordered<T>);
   { -t } -> std::convertible_to<std::remove_cvref_t<T>>;
};

/// Implementation forwarding to @c std::abs.
template <HasStdAbsSupport T>
constexpr decltype(auto) norm(T && t) noexcept(noexcept(std::abs(std::forward<T>(t))))
{
    return std::abs(std::forward<T>(t));
}

/// Identity for unsigned builtin types.
template <typename T>
constexpr decltype(auto) norm(T && t) noexcept(noexcept(std::forward<T>(t)))
    requires(std::is_unsigned_v<std::remove_cvref_t<T>>)
{
    return std::forward<T>(t);
}

/// Manually implemented version of abs for types without native support by the standard library.
///
/// Implementation assumes that both @c t and @c -t can be represented by @tparam T. Function returns unspecified
/// results otherwise.
template <typename T>
constexpr auto norm(T && t)
    requires(ManuallyNormable<T> and not HasStdAbsSupport<T> and not std::is_unsigned_v<std::remove_cvref_t<T>>)
{
    if (t < std::remove_cvref_t<T>{}) {
        // For small builtin types, unary minus is computed via promotion to int. Therefore, we need to convert
        // back to T explicitly.
        // This cast is problematic for the lowest values of T. The same issue exists for std::abs (with e.g. int) where
        // this exact setting is specified as UB. We left it as unspecified which at least justifies the conversion.
        return static_cast<std::remove_cvref_t<T>>(-std::forward<T>(t));
    }
    return std::forward<T>(t);
}

/// Implementation of norm based on the respective member function.
template <typename T>
requires(requires(T && t) { {t.norm()}; })
constexpr auto norm(T && t) noexcept(noexcept(std::forward<T>(t).norm()))
{
    return std::forward<T>(t).norm();
}

/// Checks for types having a @c norm overload: either by any of the default implementations above or via ADL.
template <typename T>
concept Normable = requires(T t) {
    { norm(t) };
};

/// Standard CPO according to N4381 for selecting an appropriate @c norm via ADL.
///
/// This is implemented as class and not as a lambda in order to forward the exception specification correctly.
class NormFn {
public:
    template <Normable T>
    constexpr decltype(auto) operator()(T && t) const noexcept(noexcept(norm(std::forward<T>(t))))
    {
        return norm(std::forward<T>(t));
    }
};

}

/// CPO for calling the correct norm overload.
constexpr inline auto const & norm = math_details::NormFn{};

/// Return type of invoking @c norm on @tparam T.
template <typename T>
using NormValue = decltype(norm(std::declval<T>()));

}
