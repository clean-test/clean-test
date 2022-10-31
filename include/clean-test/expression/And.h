// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ShortCircuitOperator.h"
#include "Lift.h"

#include <functional>
#include <ostream>

namespace clean_test::expression {

class And : public std::logical_and<> {
public:
    static constexpr auto short_circuit_when = false;
    static constexpr auto description = std::string_view{"and"};
};

template <typename L, typename R>
requires(
    (BasicExpression<L> or BasicExpression<R>)
    and requires(LiftValue<L> const & l, LiftValue<R> const & r) { {l and r}; }
)
constexpr auto operator and(L && lhs, R && rhs)
{
    return make_short_circuit_operator<And>(lift(std::forward<L>(lhs)), lift(std::forward<R>(rhs)));
}

}
