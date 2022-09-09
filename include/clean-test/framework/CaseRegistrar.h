// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseRunner.h"
#include "Name.h"
#include "ObservationSetup.h"
#include "ObserverFwd.h"
#include "Registry.h"
#include "SuiteRegistrar.h"
#include "SuiteRegistrationSetup.h"

#include <clean-test/utils/FwdCapture.h>
#include <clean-test/utils/Initializes.h>
#include <clean-test/utils/Pack.h>
#include <clean-test/utils/Printable.h>
#include <clean-test/utils/ScopeGuard.h>

#include <algorithm>
#include <ranges>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace clean_test::framework {
namespace case_registrar_details {

template <typename T>
using ReferenceWrapper = std::reference_wrapper<T>;

template <typename T>
class ValueWrapper {
public:
    template <typename... Args>
    constexpr ValueWrapper(Args &&... args) : m_data{std::forward<Args>(args)...}
    {}

    constexpr T & get() {
        return m_data;
    }

private:
    T m_data;
};

template <typename T>
using DataWrapper = std::conditional_t<
    std::is_lvalue_reference_v<T>,
    ReferenceWrapper<std::remove_reference_t<T>>,
    ValueWrapper<std::remove_reference_t<T>>>;


template <typename... Data, GenericCaseRunner<Data...> Runner>
constexpr /*CaseRunner<Data...>*/ auto load_runner(Runner && runner)
{
    if constexpr (CaseRunner<Runner, Data...>) {
        return std::forward<Runner>(runner);
    } else {
        return [fwd = utils::fwd_capture(std::forward<Runner>(runner))]<typename... Sample>(
                    execute::Observer & observe,
                    Sample &&... sample) mutable
            requires(sizeof...(Sample) == sizeof...(Data))
        {
            auto const setup = framework::ObservationSetup{observe};
            auto & [run] = fwd;
            run(std::forward<decltype(sample)>(sample)...);
        };
    }
}

constexpr auto make_name_generator()
{
    return [n = std::size_t{}]<typename Sample>(Sample const & sample) mutable {
        auto result = [&] {
            if constexpr (utils::Printable<Sample>) {
                auto buffer = std::ostringstream{};
                buffer << sample;
                return std::move(buffer).str();
            } else {
                return std::to_string(n);
            }
        }();
        ++n;
        return result;
    };
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Facility for registration of a test-case with a predefined name.
///
/// Registration is done through assignment in order to mimic variable initialization syntax. Implementation is split
/// into two specializations:
///  - standard test-cases: with empty template parameter pack
///  - data driven / parameterized test-cases: with container type of samples as single template argument.
template <CaseData... Data> requires(sizeof...(Data) <= 1)
class CaseRegistrar;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Facility for registration of standard (non data-driven) test-cases.
template <>
class CaseRegistrar<> {
public:
    /// C'tor from given name @p t.
    template <utils::Initializes<Name> T>
    constexpr explicit CaseRegistrar(T && t) : m_name{std::forward<T>(t)}
    {}

    /// Combined c'tor: setup name @p t as above and directly register (i.e. assign) @p runner.
    template <utils::Initializes<Name> T, GenericCaseRunner<> Runner>
    constexpr CaseRegistrar(T && t, Runner && runner) : CaseRegistrar{std::forward<T>(t)}
    {
        *this = std::forward<Runner>(runner);
    }

    /// Register @p runner test-case i.e. statically store it and register for later test-execution.
    template <GenericCaseRunner<> Runner>
    CaseRegistrar const & operator=(Runner && runner) const;

    /// Continuation operator for nested suite registration.
    template <utils::Initializes<Name> L, std::convertible_to<CaseRegistrar> R>
    friend constexpr auto operator/(L && l, R && r)
    {
        return CaseRegistrar{Name{std::forward<L>(l)} / std::forward<R>(r).m_name};
    }

    template <CaseData Data, utils::Initializes<CaseRegistrar> Case>
    friend constexpr auto operator|(Data && data, Case && c)
    {
        return CaseRegistrar<Data>{std::forward<Case>(c).m_name, std::forward<Data>(data)};
    }

private:
    Name m_name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// Facility for registration of data-driven / parameterized test-cases.
///
/// The data samples are obtained from container @tparam Data.
template <CaseData Data>
class CaseRegistrar<Data> {
    using StoredData = case_registrar_details::DataWrapper<Data>;
public:
    /// C'tor from given name @p t.
    template <utils::Initializes<Name> T, utils::Initializes<Data> DataInit>
    constexpr explicit CaseRegistrar(T && t, DataInit && data) :
        m_name{std::forward<T>(t)}, m_data{std::make_shared<StoredData>(std::forward<DataInit>(data))}
    {}

    /// Combined c'tor: setup name @p t as above and directly register (i.e. assign) @p runner.
    template <utils::Initializes<Name> T, utils::Initializes<Data> Samples, GenericCaseRunner<Data> Runner>
    constexpr CaseRegistrar(T && t, Samples && samples, Runner && runner) :
        CaseRegistrar{std::forward<T>(t), std::forward<Samples>(samples)}
    {
        *this = std::forward<Runner>(runner);
    }

    /// Register @p runner test-case i.e. statically store it and register for later test-execution.
    template <GenericCaseRunner<Data> Runner>
    CaseRegistrar const & operator=(Runner && runner) const;

    /// Continuation operator for nested suite registration.
    template <utils::Initializes<Name> L, std::convertible_to<CaseRegistrar> R>
    friend auto operator/(L && l, R && r)
    {
        auto name = Name{std::forward<L>(l)} / r.m_name;
        return CaseRegistrar{std::move(name), std::forward<R>(r).m_data};
    }

private:
    CaseRegistrar(Name name, std::shared_ptr<StoredData> data) :
        m_name{std::move(name)}, m_data{std::move(data)}
    {}

    Name m_name;
    std::shared_ptr<StoredData> m_data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Deduction guides: We need special care to distinguish between 2-arg c'tors with data / runner.
template <typename T>
CaseRegistrar(T &&) -> CaseRegistrar<>;
template <typename T, CaseData Data>
CaseRegistrar(T &&, Data &&) -> CaseRegistrar<Data>;
template <typename T, CaseData Data, GenericCaseRunner<Data> Runner>
CaseRegistrar(T &&, Data &&, Runner &&) -> CaseRegistrar<Data>;
template <typename T, GenericCaseRunner<> Runner>
CaseRegistrar(T &&, Runner &&) -> CaseRegistrar<>;

/// The standard case (without data) provides an explicit instantiation in the corresponding translation unit.
extern template class CaseRegistrar<>;

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <GenericCaseRunner<> Runner>
CaseRegistrar<> const & CaseRegistrar<>::operator=(Runner && runner) const
{
    auto const & setup = suite_registration_setup();
    auto safe_runner = case_registrar_details::load_runner<>(std::forward<Runner>(runner));
    // The standard case: non-data driven tests (just an observer as argument).
    registry().emplace_back(
        setup / m_name, std::make_unique<ConcreteCaseRunner<decltype(safe_runner)>>(std::move(safe_runner)));
    return *this;
}

template <CaseData Data>
template <GenericCaseRunner<Data> Runner>
CaseRegistrar<Data> const & CaseRegistrar<Data>::operator=(Runner && runner) const
{
    SuiteRegistrar{m_name} = [&] {
        auto safe_runner = case_registrar_details::load_runner<Data>(std::forward<Runner>(runner));
        auto const shared_runner
            = std::make_shared<case_registrar_details::DataWrapper<decltype(safe_runner)>>(std::move(safe_runner));
        auto name_generator = case_registrar_details::make_name_generator();

        auto register_case = [&]<typename Sample>(Sample && sample) {
            auto capture_data = [this]() -> std::shared_ptr<StoredData> {
                if constexpr (not std::is_lvalue_reference_v<Sample>) {
                    return {}; // no data capture necessary for rvalue samples.
                }
                return m_data;
            };
            // Note: we need to build the registrar (including the name) before forwarding the sample into the runner.
            auto name = name_generator(sample);
            CaseRegistrar<>{std::move(name)} = [fwd = utils::fwd_capture(std::forward<Sample>(sample)),
                                                shared_runner,
                                                data = capture_data()](execute::Observer & observer) mutable {
                auto & [s] = fwd;
                shared_runner->get()(observer, std::forward<Sample>(s));
            };
        };

        if constexpr (std::ranges::range<Data>) {
            auto & data = m_data->get();
            auto const end = std::end(data);
            for (auto begin = std::begin(data); begin != end; ++begin) {
                register_case(*begin);
            }
        } else if constexpr (utils::Tuple<Data>) {
            std::apply([&](auto &&... sample) constexpr { (register_case(sample), ...); }, m_data->get());
        } else {
            static_assert([](auto &&...) constexpr { return false; }());
        }
    };
    return *this;
}

}
