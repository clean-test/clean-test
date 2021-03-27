// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "StandardOperator.h"

#include <functional>
#include <ostream>

namespace clean_test::expression {

class Not : public std::logical_not<> {
public:
    template <typename T>
    std::ostream & describe(std::ostream & out, T const & t)
    {
        return out << "not " << t;
    }
};

template <BasicExpression T>
constexpr auto operator not(T const & expression)
{
    return make_standard_operator<Not>(expression);
}

}
