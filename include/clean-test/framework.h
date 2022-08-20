// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "framework/CaseRegistrar.h"
#include "framework/Expect.h"
#include "framework/SuiteRegistrar.h"
#include "framework/Tag.h"
#include "framework/ObserverFwd.h"

namespace clean_test {

using framework::expect;

using Observer = execute::Observer;
using Suite = framework::SuiteRegistrar;
using Tag = framework::Tag;
using Test = framework::CaseRegistrar;

constexpr inline auto asserted = framework::Asserted{};
constexpr inline auto flaky = framework::Flaky{};

namespace literals {

[[nodiscard]] inline auto operator""_test(char const * const name, std::size_t const size)
{
    return Test{framework::Name{std::string{name, size}}};
}

[[nodiscard]] inline auto operator""_suite(char const * const name, std::size_t const size)
{
    return Suite{framework::Name{std::string{name, size}}};
}

[[nodiscard]] constexpr auto operator""_tag(char const * const name, std::size_t const size)
{
    return Tag{std::string_view{name, size}};
}

}

}
