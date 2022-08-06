// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"
#include "Clause.h"

#include <clean-test/utils/RemoveRvalueReference.h>

namespace clean_test::expression {

/// Lift value @p t to a unit value of @c Clause<T> (iff needed).
///
/// This utility helps importing an arbitrary value @p t into the realm of expressions.
template <typename T>
constexpr decltype(auto) lift(T && t) noexcept
{
    if constexpr (BasicExpression<T>) {
        return std::forward<T>(t);
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
