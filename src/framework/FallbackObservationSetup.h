// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <framework/ObserverFwd.h>

#include <utility>

namespace clean_test::framework {

/// Scoped wrapper to maintain a (global) fallback observer.
///
/// This so called fallback observer is used as a last resort observer by the framework (i.e. client code). This can
/// only ever happen if the user provides a test-case with explicit @c Observer argument, but misses to pass this
/// observer to an @c expect call. In this situation, the automatic observer detection falls back to this observer
/// (rather than crashing).
///
/// A scoped @c FallbackObservationSetup is supposed to be held by a main-like functionality during the entire
/// evaluation of any client test-case.
class FallbackObservationSetup {
public:
    using Observer = execute::Observer;

    /// Setup @p o as fallback observer for the lifetime of the created object.
    explicit FallbackObservationSetup(Observer & o) : m_reset{std::exchange(observer(), std::addressof(o))}
    {}

    /// Reset fallback observer (as it was before the lifetime of this object).
    ~FallbackObservationSetup()
    {
        observer() = m_reset;
    }

    /// Access (globally) managed fallback observer.
    static inline Observer * & observer();

    // non-copyable and non-movable
    FallbackObservationSetup(FallbackObservationSetup &&) = delete;
    FallbackObservationSetup & operator=(FallbackObservationSetup &&) = delete;
    FallbackObservationSetup(FallbackObservationSetup const &) = delete;
    FallbackObservationSetup & operator=(FallbackObservationSetup const &) = delete;

private:
    Observer * m_reset; //!< a previously encountered observer.
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

FallbackObservationSetup::Observer * & FallbackObservationSetup::observer()
{
    static Observer * singleton = nullptr;
    return singleton;
}

}
