// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "StandardOperator.h"

#include <ostream>
#include <utility>

namespace clean_test::expression {

class Affirm {
public:
    template <typename T>
    constexpr auto operator()(T && t) const noexcept(noexcept(+std::forward<T>(t))) -> decltype(+std::forward<T>(t))
    {
        return +std::forward<T>(t);
    }

    template <typename T>
    std::ostream & describe(std::ostream & out, T const & t) const
    {
        return out << '+' << t;
    }
};

template <BasicExpression T>
constexpr auto operator+(T && expression)
{
    return make_standard_operator<Affirm>(std::forward<T>(expression));
}

}
