// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstdint>

namespace clean_test::execute {

/// Outcome category of a tested user-expectation.
///
/// Wraps an underlying enum in order to provide an enum-class with extra methods.
class ObservationStatus {
public:
    enum Value : std::uint8_t {
        pass = 0, //!< Expectation satisfied, i.e. no failure
        fail_flaky = 1, //!< Expectation violated, but failure was expected.
        fail = 2, //!< Expectation violated. Any observation in this state makes a case fail.
    };

    /// Total number of supported states.
    constexpr static inline std::size_t num_values = 3ul;

    /// Detailed c'tor.
    constexpr explicit(false) ObservationStatus(Value const value) : m_value{value}
    {}

    /// Convert to underlying enum (and implicitly potentially further to underlying integer type).
    constexpr explicit(false) operator Value() const
    {
        return m_value;
    }

private:
    Value m_value;
};

}
