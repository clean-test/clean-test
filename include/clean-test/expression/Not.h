// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <ostream>

namespace clean_test::expression {

template <BasicExpression T>
class NotEvaluation;

/// Unary negation of clauses.
template <BasicExpression T>
class Not : public Base {
public:
    using Value = decltype(not std::declval<typename T::Value>());
    using Evaluation = NotEvaluation<T>;

    constexpr explicit Not(T const & expression) : m_expression{expression}
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
        return Evaluation{*this};
    }

    friend std::ostream & operator<<(std::ostream & out, Not const & n)
    {
        return out << n.evaluation();
    }

private:
    friend class NotEvaluation<T>;

    T const m_expression;
};

template <BasicExpression T>
constexpr Not<T> operator not(T const & expression)
{
    return Not<T>{expression};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression T>
class NotEvaluation {
public:
    constexpr explicit NotEvaluation(Not<T> const & n);

    [[nodiscard]] constexpr decltype(auto) value() const
    {
        return m_value;
    }

    [[nodiscard]] constexpr explicit operator bool() const
    {
        return static_cast<bool>(value());
    }

    friend std::ostream & operator<<(std::ostream & out, NotEvaluation<T> const & ne)
    {
        return out << "( not " << ne.m_evaluation << " )";
    }

private:
    typename T::Evaluation m_evaluation;
    typename Not<T>::Value m_value;
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression T>
constexpr NotEvaluation<T>::NotEvaluation(Not<T> const & n) :
    m_evaluation{n.m_expression.evaluation()}, m_value{not m_evaluation.value()}
{}

}
