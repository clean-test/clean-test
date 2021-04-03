// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ShortCircuitOperator.h"
#include "Lift.h"

#include <functional>
#include <ostream>

namespace clean_test::expression {

class Or : public std::logical_or<> {
public:
    static constexpr auto short_circuit_when = true;
    static constexpr auto description = std::string_view{"or"};
};

template <typename L, typename R> requires(BasicExpression<L> or BasicExpression<R>)
constexpr auto operator or(L && lhs, R && rhs)
{
    return make_short_circuit_operator<Or>(lift(std::forward<L>(lhs)), lift(std::forward<R>(rhs)));
}

}
