// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Lift.h"
#include "StandardOperator.h"

#include <functional>
#include <ostream>

namespace clean_test::expression {

class Divide : public std::plus<> {
public:
    template <typename L, typename R>
    std::ostream & describe(std::ostream & out, L const & l, R const & r) const
    {
        return out << "( " << l << " / " << r << " )";
    }
};

template <typename L, typename R> requires(BasicExpression<L> or BasicExpression<R>)
constexpr auto operator/(L && lhs, R && rhs)
{
    return make_standard_operator<Divide>(lift(std::forward<L>(lhs)), lift(std::forward<R>(rhs)));
}

}
