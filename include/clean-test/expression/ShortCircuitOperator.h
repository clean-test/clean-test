// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <ostream>
#include <optional>

namespace clean_test::expression {

template <typename Operator, BasicExpression L, BasicExpression R>
class ShortCircuitEvaluation;

/// Utility for a common implementation of short circuit expressions, i.e. @c and and @c or.
///
/// An @c Operator requires the following "interface":
///  - a call operator (unevaluated, just for identifying the corresponding resulting @c Value type)
///  - a @c description (i.e. the string for joining the visual representations of the left- and right-hand sides).
///  - a @c short_circuit_when (bool-convertible) criterion: if the left-hand side compares equal to this criterion,
///    the evaluation of the right-hand side is skipped (short circuit evaluation).
template <typename Operator, BasicExpression L, BasicExpression R> requires(only_values<L, R>)
class ShortCircuitOperator : public ExpressionBase<ShortCircuitOperator<Operator, L, R>> {
public:
    using Value
        = decltype(std::declval<Operator>()(std::declval<typename L::Value>(), std::declval<typename R::Value>()));
    using Evaluation = ShortCircuitEvaluation<Operator, L, R>;

    constexpr ShortCircuitOperator(std::convertible_to<L> auto && lhs, std::convertible_to<R> auto && rhs) :
        m_lhs{std::forward<decltype(lhs)>(lhs)}, m_rhs{std::forward<decltype(rhs)>(rhs)}
    {}

    [[nodiscard]] constexpr Evaluation evaluation() const
    {
        return ShortCircuitEvaluation{*this};
    }

private:
    friend Evaluation;

    L const m_lhs;
    R const m_rhs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Cached evaluation of a short circuit operator.
template <typename Operator, BasicExpression L, BasicExpression R>
class ShortCircuitEvaluation : public EvaluationBase<ShortCircuitEvaluation<Operator, L, R>>, private Operator {
public:
    using Value = typename ShortCircuitOperator<Operator, L, R>::Value;

    constexpr explicit ShortCircuitEvaluation(ShortCircuitOperator<Operator, L, R> const & sco);

    [[nodiscard]] constexpr Value const & value() const
    {
        return (m_rhs ? *m_rhs_value : m_lhs_value);
    }

    friend std::ostream & operator<<(std::ostream & out, ShortCircuitEvaluation const & expr)
    {
        return expr.print_to(out);
    }

private:
    using Operator::description;
    using Operator::short_circuit_when;
    std::ostream & print_to(std::ostream & out) const;

    typename L::Evaluation m_lhs;
    Value m_lhs_value;

    std::optional<typename R::Evaluation> m_rhs;
    std::optional<Value> m_rhs_value;
};

/// Factory for selecting appropriate template arguments for the returned @c ShortCircuitOperator based on arguments.
///
/// Necessary utility as long as there is no partial CTAD.
template <typename Operator, typename L, typename R>
constexpr auto make_short_circuit_operator(L && lhs, R && rhs)
{
    return ShortCircuitOperator<Operator, std::remove_reference_t<L>, std::remove_reference_t<R>>{
        std::forward<L>(lhs), std::forward<R>(rhs)};
}

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Operator, BasicExpression L, BasicExpression R>
constexpr ShortCircuitEvaluation<Operator, L, R>::ShortCircuitEvaluation(
    ShortCircuitOperator<Operator, L, R> const & sco) :
    m_lhs{sco.m_lhs.evaluation()},
    m_lhs_value{static_cast<Value>(m_lhs.value())},
    m_rhs{static_cast<bool>(m_lhs) != short_circuit_when ? std::optional{sco.m_rhs.evaluation()} : std::nullopt},
    m_rhs_value{m_rhs ? std::optional{static_cast<Value>(m_rhs->value())} : std::nullopt}
{}

template <typename Operator, BasicExpression L, BasicExpression R>
std::ostream & ShortCircuitEvaluation<Operator, L, R>::print_to(std::ostream & out) const
{
    out << "( " << m_lhs << ' ' << description << ' ';

    // Check whether rhs was ever considered (due to short circuiting). Otherwise it might be unsafe to print it.
    if (m_rhs_value) {
        out << *m_rhs;
    } else {
        out << "<unknown>";
    }

    return out << " )";
}

}
