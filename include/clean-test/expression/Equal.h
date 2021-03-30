// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"
#include "Lift.h"

#include <ostream>

namespace clean_test::expression
{

/// Equality comparison of clauses.
template <BasicExpression L, BasicExpression R>
class Equal : public Base {
public:
    using Value = bool;
    using Evaluation = Equal;

    constexpr Equal(L const & lhs, R const & rhs) : m_lhs{lhs}, m_rhs{rhs}
    {}

    [[nodiscard]] constexpr explicit operator bool() const
    {
        if constexpr (requires { m_lhs.value(); m_rhs.value();}) {
            return (m_lhs.value() == m_rhs.value());
        } else {
            return evaluate(m_lhs) == evaluate(m_rhs);
        }
    }

    friend std::ostream & operator<<(std::ostream & out, Equal const & expr)
    {
        return out << "( " << expr.m_lhs << " == " << expr.m_rhs << " )";
    }

    constexpr auto & evaluation() const
    {
        return *this;
    }

    constexpr auto value() const
    {
        return static_cast<bool>(*this);
    }

private:
    L const m_lhs;
    R const m_rhs;
};

template <typename L, BasicExpression R> requires (not BasicExpression<L>)
Equal(L, R) -> Equal<Clause<L>, R>;

template <BasicExpression L, typename R> requires (not BasicExpression<R>)
Equal(L, R) -> Equal<L, Clause<R>>;

template <typename L, typename R> requires (BasicExpression<L> or BasicExpression<R>)
constexpr auto operator==(L const & lhs, R const & rhs)
{
    return Equal{lift(lhs), lift(rhs)};
}

}
