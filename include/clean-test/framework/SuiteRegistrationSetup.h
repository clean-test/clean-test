// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

namespace clean_test::framework {

class Name;

/// The currently registered test-suite name.
///
/// This setup defines the shared state different registered suites communicate with.
Name & suite_registration_setup();

}
