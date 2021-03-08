// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <ostream>

namespace clean_test::expression {

/// Unary negation of clauses.
template <BasicExpression T>
class Not : public Base {
public:
    constexpr Not(T const & expression) : m_expression{expression}
    {}

    [[nodiscard]] constexpr explicit operator bool() const
    {
        return not evaluate(m_expression);
    }

    friend std::ostream & operator<<(std::ostream & out, Not const & n)
    {
        return out << "( not " << n.m_expression << " )";
    }

private:
    T const m_expression;
};

template <typename T> requires (BasicExpression<T>)
constexpr Not<T> operator not(T const & expression)
{
    return Not{expression};
}

//template <typename T> requires (BasicExpression<T>)
//constexpr auto operator not(T const & expression)
//{
//    return Not{expression};
//}

}
