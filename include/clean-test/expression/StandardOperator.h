// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <tuple>
#include <type_traits>

namespace clean_test::expression {

template <typename Operator, BasicExpression... Expression> requires(only_values<Expression...>)
class StandardOperatorEvaluation;

template <typename Operator, BasicExpression... Expression> requires(only_values<Expression...>)
class StandardOperator final : public ExpressionBase<StandardOperator<Operator, Expression...>> {
public:
    using Value = std::remove_cvref_t<decltype(std::declval<Operator>().operator()(
        std::declval<typename Expression::Value>()...))>;
    using Evaluation = StandardOperatorEvaluation<Operator, Expression...>;

    constexpr explicit(sizeof...(Expression) == 1) StandardOperator(std::convertible_to<Expression> auto &&... expr) :
        m_expression{std::forward<decltype(expr)>(expr)...}
    {}

    constexpr auto evaluation() const
    {
        return Evaluation{*this};
    }

private:
    friend Evaluation;

    std::tuple<Expression const...> m_expression;
};

template <typename Operator, BasicExpression... Expression> requires(only_values<Expression...>)
class StandardOperatorEvaluation final : public EvaluationBase<StandardOperatorEvaluation<Operator, Expression...>>,
                                         private Operator {
public:
    // clang-format off
    constexpr explicit(sizeof...(Expression) == 1)
    StandardOperatorEvaluation(StandardOperator<Operator, Expression...> const & op) :
        m_evaluation{
            [&op]<std::size_t... idx>(std::index_sequence<idx...>) constexpr {
                return std::tuple{std::get<idx>(op.m_expression).evaluation()...};
            }(std::index_sequence_for<Expression...>{})
        },
        m_value{
            [this]<std::size_t... idx>(std::index_sequence<idx...>) constexpr -> decltype(auto) {
                auto const & evaluation = m_evaluation;
                return this->Operator::operator()(std::get<idx>(evaluation).value()...);
            }(std::index_sequence_for<Expression...>{})
        }
    {}
    // clang-format on

    constexpr typename StandardOperator<Operator, Expression...>::Value const & value() const
    {
        return m_value;
    }

    friend std::ostream & operator<<(std::ostream & out, StandardOperatorEvaluation const & evaluation)
    {
        // clang-format off
        return [&evaluation, &out]<std::size_t... idx>(std::index_sequence<idx...>) -> decltype(auto)
        {
            return evaluation.describe(out, std::get<idx>(evaluation.m_evaluation)...);
        }(std::index_sequence_for<Expression...>{});
        // clang-format on
    }

private:
    std::tuple<typename std::remove_cvref_t<Expression>::Evaluation...> m_evaluation;
    typename StandardOperator<Operator, Expression...>::Value const m_value;
};

/// Factory for selecting appropriate template arguments for the returned @c StandardOperator based on @c expression.
///
/// Necessary utility as long as there is no partial CTAD.
template <typename Operator, BasicExpression... Expression>
constexpr auto make_standard_operator(Expression&&... expression)
{
    return StandardOperator<Operator, std::remove_reference_t<Expression>...>{
        std::forward<decltype(expression)>(expression)...};
}

}
