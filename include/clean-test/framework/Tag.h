// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <string_view>

namespace clean_test::framework {

/// Label type for test-suites and cases.
class Tag {
public:
    constexpr explicit Tag(std::string_view const name) : m_name{name} {}

    constexpr explicit operator std::string_view() const
    {
        return m_name;
    }

private:
    std::string_view m_name;
};

}
