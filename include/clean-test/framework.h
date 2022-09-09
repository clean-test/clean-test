// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "framework/CaseRegistrar.h"
#include "framework/Expect.h"
#include "framework/ObserverFwd.h"
#include "framework/SuiteRegistrar.h"
#include "framework/Tag.h"

namespace clean_test {

using framework::expect;

using Observer = execute::Observer;
using Suite = framework::SuiteRegistrar;
using Tag = framework::Tag;

// No clang release so far supports CTAD for template aliases. We include a workaround.
// MSVC seems to struggle with it too.
#if (defined(__clang_major__) and __clang_major__ <= 100) or \
    (defined(_MSC_VER) and _MSC_VER < 10'000)

template <typename... Data>
class Test : public framework::CaseRegistrar<Data...> {
public:
    using Base = framework::CaseRegistrar<Data...>;
    using Base::CaseRegistrar;

    template <typename T>
    requires(std::is_assignable_v<Base const &, T>)
    Test const & operator=(T && t) const
    {
        Base::operator=(std::forward<T>(t));
        return *this;
    }
};

template <typename T>
Test(T &&) -> Test<>;
template <typename T, framework::CaseData Rng, typename... Args>
Test(T &&, Rng&&, Args&&...) -> Test<Rng>;
template <typename T, framework::GenericCaseRunner<> Runner>
Test(T &&, Runner&&) -> Test<>;

#else

template <typename... Args>
using Test = framework::CaseRegistrar<Args...>;

#endif

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
