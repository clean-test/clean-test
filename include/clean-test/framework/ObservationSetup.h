// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ObserverFwd.h"

namespace clean_test::framework {

/// The (thread-local) observation setup.
///
/// This setup is used for reporting violated expectations when no other @c Observer is passed explicitly.
execute::Observer * & observation_setup();

}
