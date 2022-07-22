// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <version>

#if defined(__cpp_lib_source_location) and __cpp_lib_source_location >= 201907L
# include <source_location>
#else
# include <cstdint>
#endif

namespace clean_test::utils {

#if defined(__cpp_lib_source_location) and __cpp_lib_source_location >= 201907L
using SourceLocation = std::source_location;
#else

/// Pre C++20 drop-in replacement of std::source_location.
class SourceLocation {
public:
    [[nodiscard]] constexpr auto file_name() const noexcept
    {
        return m_file;
    }

    [[nodiscard]] constexpr auto function_name() const noexcept
    {
        return m_function;
    }

    [[nodiscard]] constexpr auto line() const noexcept
    {
        return m_line;
    }

    [[nodiscard]] constexpr std::uint_least32_t column() const noexcept
    {
        return 0;
    }

    // Note: Passing via arguments in order to evaluate the default args at the call-sites.
    [[nodiscard]] static constexpr SourceLocation current(
        char const * file =
#if __has_builtin(__builtin_FILE)
            __builtin_FILE()
#else
            "unknown"
#endif
            ,
        char const * function =
#if __has_builtin(__builtin_FUNCTION)
            __builtin_FUNCTION()
#else
            "unknown"
#endif
            ,
        std::uint_least32_t line =
#if __has_builtin(__builtin_LINE)
            __builtin_LINE()
#else
            0
#endif
            ) noexcept
    {
        return {file, function, line};
    }

private:
    constexpr SourceLocation(char const * file, char const * function, std::uint_least32_t const line)
        : m_file{file}, m_function{function}, m_line{line}
    {}

    char const * m_file;
    char const * m_function;
    std::uint_least32_t m_line = 0;
};

#endif
} // clean_test::utils
