// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "StandardOperator.h"
#include "Lift.h"

#include <functional>
#include <ostream>

namespace clean_test::expression {

class BitwiseNot : public std::bit_not<> {
public:
    template <typename T>
    std::ostream & describe(std::ostream & out, T const & t) const
    {
        return out << '~' << t;
    }
};

template <BasicExpression T> requires(requires(typename T::Value const & v) { {~v}; })
constexpr auto operator~(T && expression)
{
    return make_standard_operator<BitwiseNot>(std::forward<T>(expression));
}


}
