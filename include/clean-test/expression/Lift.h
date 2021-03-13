// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"
#include "Clause.h"

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


}
