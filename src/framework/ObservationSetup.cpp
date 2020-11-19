// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "framework/ObservationSetup.h"

#include "framework/ObserverFwd.h"

namespace clean_test::framework {

execute::Observer * & observation_setup()
{
    thread_local execute::Observer * observer = nullptr;
    return observer;
}

}
