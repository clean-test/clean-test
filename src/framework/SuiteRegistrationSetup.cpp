// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "framework/SuiteRegistrationSetup.h"

#include "framework/Name.h"

namespace clean_test::framework {

Name & suite_registration_setup()
{
    static auto name = Name{};
    return name;
}

}
