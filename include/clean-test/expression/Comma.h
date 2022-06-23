// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "StandardOperator.h"

#include <ostream>
#include <utility>

namespace clean_test::expression {

class Comma {
public:
    template <typename L, typename R>
    constexpr auto operator()(L && l, R && r) const noexcept(noexcept(std::forward<L>(l), std::forward<R>(r)))
        -> decltype(std::forward<L>(l), std::forward<R>(r))
    {
#ifdef _MSC_VER
#pragma warning(suppress: 4834)
#endif
        return (std::forward<L>(l), std::forward<R>(r));
    }

    template <typename L, typename R>
    std::ostream & describe(std::ostream & out, L const & lhs, R const & rhs) const
    {
        return out << lhs << ", " << rhs;
    }
};

template <BasicExpression L, BasicExpression R>
constexpr auto operator,(L && lhs, R && rhs)
{
    return make_standard_operator<Comma>(std::forward<L>(lhs), std::forward<R>(rhs));
}

}
