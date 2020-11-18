// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Name.h"
#include "SuiteRegistrationSetup.h"

#include <clean-test/utils/Initializes.h>
#include <clean-test/utils/ScopeGuard.h>

#include <type_traits>
#include <concepts>

namespace clean_test::framework {

/// Facility for registration of a test-suite with a predefined name.
///
/// Registration is done through assignment in order to mimic variable initialization syntax.
class SuiteRegistrar {
public:
    /// C'tor from given name @p t.
    template <utils::Initializes<Name> T>
    constexpr explicit SuiteRegistrar(T && t) : m_name{std::forward<T>(t)}
    {}

    /// Combined c'tor: setup name @p t as above and directly register (i.e. assign) @p generator.
    template <utils::Initializes<Name> T, std::invocable Generator>
    constexpr SuiteRegistrar(T && t, Generator && generator)
        : SuiteRegistrar{std::forward<T>(t)}
    {
        *this = std::forward<Generator>(generator);
    }

    /// Register @p generator test-suite i.e. ensure global naming setup and invoke @p generator (in this context).
    template <std::invocable Generator>
    SuiteRegistrar const & operator=(Generator && generator) const;

    /// Continuation operator for nested suite registration.
    template <utils::Initializes<Name> L, std::same_as<SuiteRegistrar> R>
    friend auto operator/(L && l, R && r)
    {
        return SuiteRegistrar{Name{std::forward<L>(l)} / std::forward<R>(r).m_name};
    }

private:
    Name m_name;
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::invocable Generator>
SuiteRegistrar const & SuiteRegistrar::operator=(Generator && generator) const
{
    auto & setup = suite_registration_setup();
    auto const cleanup = utils::ScopeGuard{[restore = setup, &use = setup] { use = restore; }};

    setup /= m_name;
    std::forward<Generator>(generator)();
    return *this;
}

}
