// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstddef>
#include <cstdint>

namespace clean_test::execute {

/// Outcome category of a tested user-expectation.
///
/// Wraps an underlying enum in order to provide an enum-class with extra methods.
class ObservationStatus {
public:
    enum Value : std::uint8_t {
        /// Expectation satisfied, i.e. no failure
        pass = 0,
        /// Expectation violated, but failure was expected.
        fail_flaky,
        /// Expectation violated. Any observation in this state makes a case fail.
        fail,
        /// Asserted expectation violated. An observation in this state makes the case fail and aborts its execution.
        fail_asserted,
    };

    /// Total number of supported states.
    constexpr static inline std::size_t num_values = 4ul;

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
