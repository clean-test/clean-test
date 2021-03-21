// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Clause.h"
#include "Lift.h"

#include <ostream>
#include <optional>

namespace clean_test::expression {

template <BasicExpression L, BasicExpression R>
class AndEvaluation;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Boolean and for two basic expressions of this framework.
template <BasicExpression L, BasicExpression R>
class And : public Base {
public:
    using Value = decltype(std::declval<typename L::Value>() and std::declval<typename R::Value>());
    using Evaluation = AndEvaluation<L, R>;

    constexpr And(L const & lhs, R const & rhs) : m_lhs{lhs}, m_rhs{rhs}
    {}

    [[nodiscard]] constexpr Value value() const
    {
        return evaluation().value();
    }

    [[nodiscard]] constexpr explicit operator bool() const
    {
        return static_cast<bool>(evaluation());
    }

    [[nodiscard]] constexpr Evaluation evaluation() const
    {
        return AndEvaluation{*this};
    }

    friend std::ostream & operator<<(std::ostream & out, And<L, R> const & a)
    {
        return out << a.evaluation();
    }

private:
    friend class AndEvaluation<L, R>;

    L const m_lhs;
    R const m_rhs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Cached evaluation of a boolean and for two basic expressions of this framework.
template <BasicExpression L, BasicExpression R>
class AndEvaluation {
public:
    using Value = typename And<L, R>::Value;

    constexpr explicit AndEvaluation(And<L, R> const & a) :
        m_lhs{a.m_lhs.evaluation()},
        m_lhs_value{static_cast<Value>(m_lhs.value())},
        m_rhs{m_lhs_value ? std::optional{a.m_rhs.evaluation()} : std::nullopt},
        m_rhs_value{m_rhs ? std::optional{static_cast<Value>(m_rhs->value())} : std::nullopt}
    {}

    [[nodiscard]] constexpr Value const & value() const
    {
        return (m_rhs ? *m_rhs_value : m_lhs_value);
    }

    [[nodiscard]] constexpr explicit operator bool() const
    {
        return static_cast<bool>(value());
    }

    friend std::ostream & operator<<(std::ostream & out, AndEvaluation const & expr)
    {
        return expr.print_to(out);
    }

private:
    std::ostream & print_to(std::ostream & out) const;

    typename L::Evaluation m_lhs;
    Value m_lhs_value;

    std::optional<typename R::Evaluation> m_rhs;
    std::optional<Value> m_rhs_value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename L, BasicExpression R> requires(not BasicExpression<L>)
And(L, R)->And<Clause<L>, R>;

template <BasicExpression L, typename R> requires(not BasicExpression<R>)
And(L, R)->And<L, Clause<R>>;

template <typename L, typename R> requires(BasicExpression<L> or BasicExpression<R>)
constexpr auto operator and(L const & lhs, R const & rhs)
{
    return And{lift(lhs), lift(rhs)};
}

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression L, BasicExpression R>
std::ostream & AndEvaluation<L, R>::print_to(std::ostream & out) const
{
    out << "( " << m_lhs << " and ";

    // Check whether rhs was ever considered (due to short circuiting). Otherwise it might be unsafe to print it.
    if (m_rhs_value) {
        out << *m_rhs;
    } else {
        out << "<unknown>";
    }

    return out << " )";
}

}
