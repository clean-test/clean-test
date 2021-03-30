// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Clause.h"
#include "Lift.h"

#include <iostream>

namespace clean_test::expression {

/// Boolean and for two basic expressions of this framework.
template <BasicExpression L, BasicExpression R>
class And : public Base {
public:
    using Value = bool;
    using Evaluation = And;

    constexpr And(L const & lhs, R const & rhs)
        : m_lhs{lhs}, m_rhs{rhs}, m_left_truth{evaluate(m_lhs)}, m_truth{m_left_truth and evaluate(m_rhs)}
    {}

    [[nodiscard]] constexpr explicit operator bool() const
    {
        return m_truth;
    }

    friend std::ostream & operator<<(std::ostream & out, And const & expr)
    {
        return expr.print_to(out);
    }

    constexpr auto value() const
    {
        return static_cast<bool>(*this);
    }

    constexpr auto& evaluation() const
    {
        return *this;
    }

private:
    std::ostream & print_to(std::ostream & out) const;

    L const m_lhs;
    R const m_rhs;

    // cached conversions to bool
    bool const m_left_truth;
    bool const m_truth;
};


template <typename L, BasicExpression R> requires (not BasicExpression<L>)
And(L, R) -> And<Clause<L>, R>;

template <BasicExpression L, typename R> requires (not BasicExpression<R>)
And(L, R) -> And<L, Clause<R>>;

template <typename L, typename R> requires (BasicExpression<L> or BasicExpression<R>)
constexpr auto operator and(L const & lhs, R const & rhs)
{
    return And{lift(lhs), lift(rhs)};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression L, BasicExpression R>
std::ostream & And<L, R>::print_to(std::ostream & out) const
{
    out << "( " << m_lhs << " and ";

    // Check whether rhs was ever considered (due to short circuiting). Otherwise it might be unsafe to print it.
    if (m_left_truth) {
        out << m_rhs;
    } else {
        out << "<unknown>";
    }

    return out << " )";
}

}
