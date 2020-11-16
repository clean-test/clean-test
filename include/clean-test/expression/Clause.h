// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <ostream>

namespace clean_test::expression {

/// The basic building block: Something we can print and convert to bool.
template <typename T>
class Clause : public Base {
public:
    constexpr explicit(false) Clause(std::convertible_to<T> auto && value) :
        m_value{std::forward<decltype(value)>(value)}
    {}

    friend std::ostream & operator<<(std::ostream & out, Clause const & ref)
    {
        return out << ref.value();
    }

    constexpr T const & value() const
    {
        return m_value;
    }

private:
    T m_value;
};

template <typename T>
Clause(T &) -> Clause<T &>;

template <typename T>
Clause(T &&) -> Clause<T>;

}
