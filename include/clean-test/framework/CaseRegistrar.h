// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Name.h"
#include "ObservationSetup.h"
#include "ObserverFwd.h"
#include "Registry.h"
#include "SuiteRegistrationSetup.h"

#include <clean-test/utils/FwdCapture.h>
#include <clean-test/utils/Initializes.h>
#include <clean-test/utils/ScopeGuard.h>

#include <string_view>
#include <type_traits>
#include <utility>

namespace clean_test::framework {

template <typename T>
concept CaseRunner = std::is_invocable_v<T, execute::Observer &>;

template <typename T>
concept GenericCaseRunner = CaseRunner<T> or std::is_invocable_v<T>;

/// Facility for registration of a test-case with a predefined name.
///
/// Registration is done through assignment in order to mimic variable initialization syntax.
class CaseRegistrar {
public:
    /// C'tor from given name @p t.
    template <utils::Initializes<Name> T>
    constexpr explicit CaseRegistrar(T && t) : m_name{std::forward<T>(t)}
    {}

    /// Combined c'tor: setup name @p t as above and directly register (i.e. assign) @p runner.
    template <utils::Initializes<Name> T, GenericCaseRunner Runner>
    constexpr CaseRegistrar(T && t, Runner && runner) : CaseRegistrar{std::forward<T>(t)}
    {
        *this = std::forward<Runner>(runner);
    }

    /// Register @p runner test-case i.e. statically store it and register for later test-execution.
    template <GenericCaseRunner Runner>
    CaseRegistrar const & operator=(Runner && runner) const;

    /// Continuation operator for nested suite registration.
    template <utils::Initializes<Name> L, std::same_as<CaseRegistrar> R>
    friend auto operator/(L && l, R && r)
    {
        return CaseRegistrar{Name{std::forward<L>(l)} / std::forward<R>(r).m_name};
    }

private:
    Name m_name;
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <GenericCaseRunner Runner>
constexpr CaseRunner auto load_runner(Runner && runner)
{
    if constexpr (CaseRunner<Runner>) {
        return std::forward<Runner>(runner);
    } else {
        return [fwd = utils::fwd_capture(std::forward<Runner>(runner))](execute::Observer & observe) {
            auto & setup = observation_setup();
            auto const ensure_setup = utils::ScopeGuard{
                [&setup, reset = std::exchange(setup, std::addressof(observe))] { std::exchange(setup, reset); }};

            auto & [runner] = fwd;
            runner();
        };
    }
}

template <GenericCaseRunner Runner>
CaseRegistrar const & CaseRegistrar::operator=(Runner && runner) const
{
    auto const & setup = suite_registration_setup();
    static auto stored_runner = load_runner(std::forward<Runner>(runner));
    registry().emplace_back(setup / m_name, std::cref(stored_runner));
    return *this;
}

}
