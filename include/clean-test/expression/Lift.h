// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"
#include "Clause.h"
#include "LazyDetector.h"

#include <clean-test/utils/RemoveRvalueReference.h>

namespace clean_test::expression {
namespace lift_details {

/// Detector for @p LazyDetector that directly forwards the result of invocating a function object.
class Invoker {
public:
    template <std::invocable<> Func>
    decltype(auto) operator()(Func && func) const {
        return std::forward<Func>(func)();
    }
};

}

/// Lift value @p t to a unit value of @c Clause<T> (iff needed).
///
/// This utility helps importing an arbitrary value @p t into the realm of expressions. This means:
///  - just forward if this is already the case, or
///  - else evaluate lazily if parameter is callable, or
///  - wrap parameter.
///
/// The constraints ensure that a lifted callable doesn't return void.
template <typename T>
requires(BasicExpression<T> or not std::invocable<T> or not std::is_same_v<void, std::invoke_result_t<T>>)
constexpr decltype(auto) lift(T && t) noexcept
{
    if constexpr (BasicExpression<T>) {
        return std::forward<T>(t);
    } else if constexpr (std::invocable<T>) {
        return LazyDetector<lift_details::Invoker, T>{std::forward<T>(t)};
    } else {
        return Clause{std::forward<T>(t)};
    }
}

/// Type returned from lifting @tparam T.
template <typename T>
using Lift = utils::RemoveRvalueReference<decltype(lift(std::declval<T>()))>;

/// Value type provided by evaluating a lifted expression of @tparam T.
template <typename T>
using LiftValue = typename std::remove_cvref_t<Lift<T>>::Value;

}
