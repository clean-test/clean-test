// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "framework/ObservationSetup.h"

#include <framework/FallbackObservationSetup.h>
#include <framework/ObserverFwd.h>

#include <utility>

namespace clean_test::framework {
namespace {

auto * & registry()
{
    thread_local ObservationSetup::Observer * observer = nullptr;
    return observer;
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

execute::Observer * & observation_setup()
{
    return registry();
}

execute::Observer & ObservationSetup::observer()
{
    if (registry() == nullptr) {
        return *FallbackObservationSetup::observer();
    }
    return *registry();
}

ObservationSetup::ObservationSetup(Observer & observer) :
    m_reset{std::exchange(registry(), std::addressof(observer))}
{}

ObservationSetup::~ObservationSetup()
{
    registry() = m_reset;
}

}
