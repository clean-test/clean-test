// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"
#include "Distance.h"
#include "Lift.h"

#include <clean-test/utils/Math.h>

#include <concepts>
#include <limits>

namespace clean_test::expression {

/// Identifies @tparam T and @tparam U as equal if they are identical after removing cvref-qualifiers.
template <typename T, typename U>
concept SameValueAs = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

/// Configuration of bounds for two values to be considered close.
///
/// Closeness checks honor the weaker of the bounds, i.e. number are considered close when they lie within any of the
/// configured tolerance bounds. The bounds are considered inclusive, e.g. numbers are close
/// when exactly @c distance.m_absolute apart.
template <typename T>
class Tolerance {
public:
    using Value = std::remove_cvref_t<T>;

    /// Absolute tolerated difference (needs to be non-negative)
    ///
    /// Depending on the absolute size of compared numbers, this tolerance might need to be raised manually,
    /// e.g. when dealing with large numbers.
    T m_absolute;

    /// Allowed error as fraction of the absolute values being compared (not percent).
    T m_relative;

    friend std::ostream & operator<<(std::ostream & out, Tolerance const & tolerance)
    {
        return out << Distance<T>{.m_absolute = tolerance.m_absolute, .m_relative = tolerance.m_relative};
    }

    /// @name Operators for comparing Distances with Tolerances.
    ///
    /// The comparison of a distance with a tolerance using any relational operator `op` holds iff the absolute or
    /// relative parts compare `op` (one suffices, @see compare_with). By nature of the definition above, `d <= t` is
    /// *not* equivalent to `not (d > t)`, e.g. `d = (0, 1)` and `t = (1, 0)`.
    ///
    /// Only those with Distance as first argument are actually implemented; the ones with Tolerance as first argument
    /// simply forward to the swapped operator (e.g. a <= b  <=>  b >= a).
    ///
    /// @{

    template <SameValueAs<T> U>
    friend constexpr auto operator<=(Distance<U> const & dist, Tolerance<T> const & tolerance) noexcept
    {
        return compare_with<std::less_equal<Value>>(dist, tolerance);
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator<(Distance<U> const & dist, Tolerance<T> const & tolerance) noexcept
    {
        return compare_with<std::less<Value>>(dist, tolerance);
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator>=(Distance<U> const & dist, Tolerance<T> const & tolerance) noexcept
    {
        return compare_with<std::greater_equal<Value>>(dist, tolerance);
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator>(Distance<U> const & dist, Tolerance<T> const & tolerance) noexcept
    {
        return compare_with<std::greater<Value>>(dist, tolerance);
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator<=(Tolerance<T> const & tolerance, Distance<U> const & dist) noexcept
    {
        return dist >= tolerance;
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator<(Tolerance<T> const & tolerance, Distance<U> const & dist) noexcept
    {
        return dist > tolerance;
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator>=(Tolerance<T> const & tolerance, Distance<U> const & dist) noexcept
    {
        return dist <= tolerance;
    }

    template <SameValueAs<T> U>
    friend constexpr auto operator>(Tolerance<T> const & tolerance, Distance<U> const & dist) noexcept
    {
        return dist < tolerance;
    }
    /// @}

private:
    template <typename Compare, typename U>
    static constexpr auto compare_with(Distance<U> const & lhs, Tolerance<T> const & rhs) noexcept
    {
        constexpr auto cmp = Compare{};
        return cmp(lhs.m_absolute, rhs.m_absolute) or cmp(lhs.m_relative, rhs.m_relative);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression L, BasicExpression R>
class ToleranceEvaluation;

template <BasicExpression Abs, BasicExpression Rel>
class ToleranceExpression final : public ExpressionBase<ToleranceExpression<Abs, Rel>> {
public:
    static_assert(std::is_same_v<
                  std::remove_cvref_t<typename std::remove_cvref_t<Abs>::Value>,
                  std::remove_cvref_t<typename std::remove_cvref_t<Rel>::Value>>);
    using Evaluation = ToleranceEvaluation<Abs, Rel>;
    using Value = Tolerance<typename std::remove_cvref_t<Abs>::Value const &>;

    constexpr ToleranceExpression(std::convertible_to<Abs> auto && abs, std::convertible_to<Rel> auto && rel) :
        m_abs{std::forward<Abs>(abs)}, m_rel{std::forward<Rel>(rel)} {};

    constexpr Evaluation evaluation() const
    {
        return Evaluation{*this};
    }

private:
    friend Evaluation;

    Abs m_abs;
    Rel m_rel;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <BasicExpression Abs, BasicExpression Rel>
class ToleranceEvaluation final : public EvaluationBase<ToleranceEvaluation<Abs, Rel>> {
public:
    using Expression = ToleranceExpression<Abs, Rel>;

    explicit constexpr ToleranceEvaluation(Expression const &);

    constexpr ToleranceEvaluation(ToleranceEvaluation && other) noexcept;

    // non-copyable and non-assignable
    ToleranceEvaluation(ToleranceEvaluation const &) = delete;
    ToleranceEvaluation & operator=(ToleranceEvaluation const &) = delete;
    ToleranceEvaluation & operator=(ToleranceEvaluation &&) = delete;

    [[nodiscard]] constexpr auto & value() const
    {
        return m_tolerance;
    }

    friend std::ostream & operator<<(std::ostream & out, ToleranceEvaluation const & ce)
    {
        return out << ce.value();
    }

private:
    typename std::remove_cvref_t<Abs>::Evaluation m_abs;
    typename std::remove_cvref_t<Rel>::Evaluation m_rel;
    typename Expression::Value m_tolerance;
};

template <BasicExpression L, BasicExpression R>
constexpr ToleranceEvaluation<L, R>::ToleranceEvaluation(Expression const & xpr) :
    m_abs{xpr.m_abs.evaluation()},
    m_rel{xpr.m_rel.evaluation()},
    m_tolerance{.m_absolute = m_abs.value(), .m_relative = m_rel.value()}
{}

template <BasicExpression L, BasicExpression R>
constexpr ToleranceEvaluation<L, R>::ToleranceEvaluation(ToleranceEvaluation && other) noexcept :
    m_abs{std::move(other.m_abs)},
    m_rel{std::move(other.m_rel)},
    m_tolerance{.m_absolute = m_abs.value(), .m_relative = m_rel.value()}
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Configure allowed inaccuracy for comparing numbers (i.e. @c Distance s).
///
/// The tolerance is evaluated lazily upon comparison with some @c Difference.
///
/// The comparison of a distance with a tolerance using any relational operator `op` holds iff the absolute or relative
/// parts compare `op` (one suffices; e.g. `d <= t` iff absolute or relative components of `d` and `t` compare `<=`).
/// This allows to express assertion for bounds that are both sufficiently small and large.
///
/// @note By nature of the definition above, `d <= t` is *not* equivalent to `not (d > t)`.
template <typename T, typename U = T> requires(std::is_convertible_v<LiftValue<U>, LiftValue<T>>)
constexpr auto tolerance(
    T && absolute = std::numeric_limits<LiftValue<T>>::epsilon(),
    U && relative = std::numeric_limits<LiftValue<U>>::epsilon())
{
    return ToleranceExpression<Lift<T>, Lift<U>>{lift(std::forward<T>(absolute)), lift(std::forward<U>(relative))};
}

}
