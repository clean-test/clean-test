// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ObserverFwd.h"

#include <clean-test/utils/SourceLocation.h>

#include <functional>
#include <sstream>

namespace clean_test::framework {

class Asserted final {};
class Flaky final {};

class ExpectationObserver {
public:
    template <typename T>
    ExpectationObserver(execute::Observer & observer, T const & what, utils::SourceLocation const & where) noexcept
        :
        m_observer{observer}, m_failed{not static_cast<bool>(what)}, m_what{encode(what)}, m_where{where}
    {}

    // movable
    ExpectationObserver(ExpectationObserver &&) = default;
    ExpectationObserver & operator=(ExpectationObserver &&) = default;

    // not copyable
    ExpectationObserver(ExpectationObserver const &) = delete;
    ExpectationObserver & operator=(ExpectationObserver const &) = delete;

    /// Manual d'tor for reporting details to @c m_observer.
    ///
    /// Throw exception on @c asserted expectation failure in order to abort further test-case execution.
    ~ExpectationObserver() noexcept(false);

    /// Stream @p other into this
    ///
    /// Honors tag-types @c Flaky and @c Asserted to mutate @c m_may_fail and @c m_abort_on_failure. Other kinds of
    /// messages are collected as user-specific output into @c m_buffer.
    template <typename Other>
    ExpectationObserver & operator<<(Other const & other);

private:
    std::string encode(auto const & what);

    std::reference_wrapper<execute::Observer> m_observer;
    bool m_failed;
    std::string m_what;
    utils::SourceLocation m_where;
    bool m_may_fail = false;
    bool m_abort_on_failure = false;
    std::ostringstream m_buffer = {}; //!< for extra user messages
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

std::string ExpectationObserver::encode(auto const & what)
{
    std::ostringstream expr;
    expr << std::boolalpha << what;
    return std::move(expr).str();
}

template <typename Other>
ExpectationObserver & ExpectationObserver::operator<<(Other const & other)
{
    if constexpr (std::is_same_v<Other, Asserted>) {
        m_abort_on_failure = true;
    } else if constexpr (std::is_same_v<Other, Flaky>) {
        m_may_fail = true;
    } else {
        m_buffer << other;
    }
    return *this;
}

}
