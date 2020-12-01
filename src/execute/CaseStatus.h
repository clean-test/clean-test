// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstdint>

namespace clean_test::execute {

/// Overall evaluation outcome of an entire test-case.
///
/// Wraps an underlying enum in order to provide an enum-class with extra methods.
class CaseStatus {
public:
    enum Value : std::uint8_t {
        pass = 0, //!< Test performed successfully.
        fail = 1, //!< Test failed (some) expectations.
        abort = 2, //!< Test aborted prematurely e.g. due to failed assertions.
    };

    /// Total number of different outcomes.
    constexpr static inline std::size_t num_values = 3ul;

    /// Detailed c'tor: Initialize from @p value.
    constexpr explicit(false) CaseStatus(Value const value) : m_value{value}
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
