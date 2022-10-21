// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"
#include "Lift.h"

#include <clean-test/utils/Math.h>
#include <clean-test/utils/RemoveRvalueReference.h>

#include <concepts>
#include <limits>

namespace clean_test::expression {

/// Measured gap between two values.
///
/// A @ Distance<T> can be compared to a @c Tolerance<T> in order to verify computations. See the latter class
/// for documentation regarding the comparison details.
///
/// @note @tparam T is not necessarily the type of compared values; @tparam T is rather the result of norming these.
template <typename T>
class Distance {
public:
    /// Absolute gap: norm of difference of values.
    T m_absolute;

    /// Relative gap: absolute gap divided by the largest absolute value being compared.
    T m_relative;

    friend std::ostream & operator<<(std::ostream & out, Distance const & distance)
    {
        return out << "{absolute: " << distance.m_absolute << ", relative: " << distance.m_relative << "}";
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression L, BasicExpression R>
class DistanceEvaluation;

template <BasicExpression L, BasicExpression R>
class DistanceExpression final : public ExpressionBase<DistanceExpression<L, R>> {
public:
    using Evaluation = DistanceEvaluation<L, R>;
    using NormValue = utils::RemoveRvalueReference<utils::NormValue<decltype(
        std::declval<typename std::remove_cvref_t<L>::Value>() - std::declval<typename std::remove_cvref_t<R>::Value>())
    >>;
    using Value = Distance<NormValue>;

    constexpr DistanceExpression(std::convertible_to<L> auto && lhs, std::convertible_to<R> auto && rhs) :
        m_lhs{std::forward<L>(lhs)}, m_rhs{std::forward<R>(rhs)} {};

    constexpr Evaluation evaluation() const
    {
        return Evaluation{*this};
    }

private:
    friend Evaluation;

    L m_lhs;
    R m_rhs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression L, BasicExpression R>
class DistanceEvaluation final : public EvaluationBase<DistanceEvaluation<L, R>> {
public:
    using Expression = DistanceExpression<L, R>;

    explicit constexpr DistanceEvaluation(Expression const & dist);

    [[nodiscard]] constexpr auto & value() const
    {
        return m_distance;
    }

    friend std::ostream & operator<<(std::ostream & out, DistanceEvaluation const & ce)
    {
        return ce.print_to(out);
    }

private:
    std::ostream & print_to(std::ostream &) const;

    static constexpr auto compute_distance(auto const & lhs, auto const & rhs);

    typename std::remove_cvref_t<L>::Evaluation m_lhs;
    typename std::remove_cvref_t<R>::Evaluation m_rhs;
    typename Expression::Value m_distance;
};

template <BasicExpression L, BasicExpression R>
constexpr DistanceEvaluation<L, R>::DistanceEvaluation(Expression const & dist) :
    m_lhs{dist.m_lhs.evaluation()},
    m_rhs{dist.m_rhs.evaluation()},
    m_distance{compute_distance(m_lhs.value(), m_rhs.value())}
{}

template <BasicExpression L, BasicExpression R>
std::ostream & DistanceEvaluation<L, R>::print_to(std::ostream & out) const
{
    return out << "distance(" << m_lhs << ", " << m_rhs << ") = " << m_distance;
}

template <BasicExpression L, BasicExpression R>
constexpr auto DistanceEvaluation<L, R>::compute_distance(auto const & lhs, auto const & rhs)
{
    auto absolute = utils::norm(lhs - rhs);
    auto relative = absolute / std::max(utils::norm(lhs), utils::norm(rhs));
    return Distance<decltype(absolute)>{
        .m_absolute = std::move(absolute),
        .m_relative = std::move(relative),
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Measure gap between @p lhs and @p rhs.
///
/// The distance is evaluated lazily upon comparison with a (configurable) @c Tolerance. There is also the convenience
/// shorthand @c is_close which uses a default tolerance.
///
/// Computing the distance requires both operands to evaluate to the same value type in order to guarantee
/// that we actually can compute the absolute and relative distances.
///
/// The comparison of a distance with a tolerance using any relational operator `op` holds iff the absolute or relative
/// parts compare `op`. @see tolerance for further details.
template <typename L, typename R>
constexpr auto distance(L && lhs, R && rhs) requires(std::is_same_v<LiftValue<L>, LiftValue<R>>)
{
    return DistanceExpression<Lift<L>, Lift<R>>{lift(std::forward<L>(lhs)), lift(std::forward<R>(rhs))};
}

}
