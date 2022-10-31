// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <functional>
#include <ostream>

namespace clean_test::expression {

template <typename Expression> requires(only_values<Expression>)
class StarEvaluation;

template <BasicExpression Operand>
class StarExpression final : public ExpressionBase<StarExpression<Operand>> {
public:
    using Value = std::remove_cvref_t<decltype(*std::declval<Operand>().evaluation().value())>;
    using Evaluation = StarEvaluation<Operand>;

    constexpr explicit StarExpression(std::convertible_to<Operand> auto && operand) :
        m_operand{std::forward<decltype(operand)>(operand)}
    {}

    constexpr auto evaluation() const
    {
        return Evaluation{m_operand};
    }

private:
    friend Evaluation;

    Operand m_operand;
};

template <typename Operand> requires(only_values<Operand>)
class StarEvaluation final : public EvaluationBase<StarEvaluation<Operand>> {
public:
    constexpr explicit StarEvaluation(auto const & expression) :
        m_evaluation{expression.evaluation()}, m_value{*m_evaluation.value()}
    {}

    constexpr auto const & value() const
    {
        return m_value;
    }

    friend std::ostream & operator<<(std::ostream & out, StarEvaluation const & evaluation)
    {
        out << evaluation.value();
        // If this value has been obtained from dereferencing a pointer, we additionally include this address
        // in the output. This avoids potentially printing data twice (think e.g. optional), while providing more
        // details where possible.
        if constexpr (std::is_pointer_v<std::remove_cvref_t<decltype(m_evaluation.value())>>) {
            out << " (@" << evaluation.m_evaluation.value() << ')';
        }
        return out;
    }

private:
    typename Operand::Evaluation m_evaluation;
    typename StarExpression<Operand>::Value m_value;
};

/// Dereference operator for @c BasicExpression s
///
/// This operator is not a StandardOperator since it's output is different: Rather than displaying "* 0x1234"
/// we want to see "42 (@0x1234)".
template <BasicExpression T> requires(requires(typename T::Value const & v) { {*v}; })
constexpr auto operator *(T && expression)
{
    return StarExpression<std::remove_cvref_t<T>>{expression};
}

}
