// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <concepts>
#include <iosfwd>
#include <type_traits>

namespace clean_test::expression {

/// Determine whether all given @tparam T s are value types.
template <typename... T>
constexpr auto only_values = (not std::is_reference_v<T> and ...);

/// Tag-type used to identify expression types.
class ExpressionTag {
};

/// Expression interface ensured via CRTP.
///
/// Inheriting @tparam Derived classes just need to provide an @c evaluation() method. The returned type is supposed
/// to contain cached details whether the expression is actually true or false. For example, a (binary) expression
/// equality expression operator needs to cache evaluations of the left- and right-hand side as well as the actual
/// @c bool of the comparison.
///
/// The evaluation is intentionally not computed upon construction in order to support expression evaluation with
/// short-circuiting (c.f. And / Or).
template <typename Derived>
class ExpressionBase : public ExpressionTag {
public:
    [[nodiscard]] constexpr explicit operator bool() const
    {
        return static_cast<bool>(evaluation());
    }

    [[nodiscard]] constexpr auto value() const
    {
        return evaluation().value();
    }

    friend std::ostream & operator<<(std::ostream & out, ExpressionBase const & b)
    {
        return out << b.evaluation();
    }

private:
    constexpr auto evaluation() const
    {
        return static_cast<Derived const &>(*this).evaluation();
    }
};

/// Evaluation interface ensured via CRTP.
template <typename Derived>
class EvaluationBase {
public:
    [[nodiscard]] constexpr explicit operator bool() const
    {
        return static_cast<bool>(static_cast<Derived const &>(*this).value());
    }
};

/// Identifies (any) expression provided in this namespace.
template <typename T>
concept BasicExpression = std::derived_from<std::remove_cvref_t<T>, ExpressionTag>;

}
