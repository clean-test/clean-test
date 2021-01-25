// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ObserverFwd.h"

namespace clean_test::framework {

/// Scoped wrapper to maintain a thread-local, test-case specific observer.
///
/// This observer is mere convenience feature for allowing the user to call @c expect without explicitly specifying
/// the observer. The execution ensures that the test-case evaluating thread will register an appropriate @c Observer
/// in a timely manner (i.e. by creating an @c ObservationSetup).
///
/// The setup is thread-local, which is sufficient for single-threaded test-cases. In case multiple user threads
/// participate in the execution of a single test-case, it is the client's responsibility to make the observer available
/// to its threads. If the user fails to do so, we use the @c FallbackObservationSetup (still registers observation,
/// but not attributed to the correct test-case).
class ObservationSetup {
public:
    using Observer = execute::Observer;

    /// Setup @p observer as @c Observation -sink for the lifetime of the created object.
    explicit ObservationSetup(Observer & observer);

    /// Restore former observer (before the lifetime of this object).
    ~ObservationSetup();

    /// Access currently setup @c Observer (or fallback observer in case there is none).
    ///
    /// This setup is used for reporting violated expectations when no other @c Observer is passed explicitly.
    static Observer & observer();

    // non-copyable and non-movable
    ObservationSetup(ObservationSetup &&) = delete;
    ObservationSetup & operator=(ObservationSetup &&) = delete;
    ObservationSetup(ObservationSetup const &) = delete;
    ObservationSetup & operator=(ObservationSetup const &) = delete;

private:
    Observer * m_reset;
};

}
