// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Clause.h"

#include <string_view>

namespace clean_test::expression::literals {

constexpr auto operator""_b(unsigned long long const num)
{
    return Clause{static_cast<bool>(num)};
}

constexpr auto operator""_s(unsigned long long const num)
{
    return Clause{static_cast<short>(num)};
}

constexpr auto operator""_us(unsigned long long const num)
{
    return Clause{static_cast<unsigned short>(num)};
}

constexpr auto operator""_c(unsigned long long const num)
{
    return Clause{static_cast<char>(num)};
}

constexpr auto operator""_sc(unsigned long long const num)
{
    return Clause{static_cast<signed char>(num)};
}

constexpr auto operator""_uc(unsigned long long const num)
{
    return Clause{static_cast<unsigned char>(num)};
}

constexpr auto operator""_w(unsigned long long const num)
{
    return Clause{static_cast<wchar_t>(num)};
}

#if defined(__cpp_lib_char8_t)
constexpr auto operator""_c8(unsigned long long const num)
{
    return Clause{static_cast<char8_t>(num)};
}
#endif

constexpr auto operator""_c16(unsigned long long const num)
{
    return Clause{static_cast<char16_t>(num)};
}

constexpr auto operator""_c32(unsigned long long const num)
{
    return Clause{static_cast<char32_t>(num)};
}

constexpr auto operator""_i(unsigned long long const num)
{
    return Clause{static_cast<int>(num)};
}

constexpr auto operator""_u(unsigned long long const num)
{
    return Clause{static_cast<unsigned int>(num)};
}

constexpr auto operator""_l(unsigned long long const num)
{
    return Clause{static_cast<long>(num)};
}

constexpr auto operator""_ul(unsigned long long const num)
{
    return Clause{static_cast<unsigned long>(num)};
}

constexpr auto operator""_ll(unsigned long long const num)
{
    return Clause{static_cast<long long>(num)};
}

constexpr auto operator""_ull(unsigned long long const num)
{
    return Clause{std::move(num)};
}

constexpr auto operator""_f(long double const num)
{
    return Clause{static_cast<float>(num)};
}

constexpr auto operator""_d(long double const num)
{
    return Clause{static_cast<double>(num)};
}

constexpr auto operator""_ld(long double const num)
{
    return Clause{static_cast<long double>(num)};
}

constexpr auto operator""_sv(char const * const buffer, std::size_t const size)
{
    return Clause{std::string_view{buffer, size}};
}

}
