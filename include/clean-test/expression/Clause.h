// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <clean-test/utils/UTF8Encoder.h>

#include <ostream>

namespace clean_test::expression {

template <typename T>
class ClauseEvaluation;

/// The basic building block: Something we can print and convert to bool.
template <typename T>
class Clause : public Base {
public:
    using Evaluation = ClauseEvaluation<T>;
    using Value = T const &;

    constexpr explicit Clause(std::convertible_to<T> auto && value) : m_value{std::forward<decltype(value)>(value)}
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

    friend std::ostream & operator<<(std::ostream & out, Clause const & ref)
    {
        return out << ref.evaluation();
    }

private:
    friend class ClauseEvaluation<T>;

    T m_value;
};

template <typename T>
Clause(T &) -> Clause<T &>;

template <typename T>
Clause(T &&) -> Clause<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// The only evaluation that really does nothing: Forward everything to the underlying clause.
template <typename T>
class ClauseEvaluation {
public:
    constexpr ClauseEvaluation(Clause<T> const & clause) : m_value{clause.m_value}
    {}

    [[nodiscard]] constexpr auto & value() const
    {
        return m_value;
    }

    [[nodiscard]] constexpr explicit operator bool() const
    {
        return static_cast<bool>(value());
    }

    friend std::ostream & operator<<(std::ostream & out, ClauseEvaluation const & ce)
    {
        return ce.print_to(out);
    }

private:
    std::ostream & print_to(std::ostream & out) const;

    typename Clause<T>::Value m_value;
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::ostream & ClauseEvaluation<T>::print_to(std::ostream & out) const
{
    if constexpr (requires { utils::UTF8Encoder::Elaborated{value()}; }) {
        out << utils::UTF8Encoder::Elaborated{value()};
    } else {
        out << value();
    }
    return out;
}

}
