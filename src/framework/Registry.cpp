// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "framework/Registry.h"

namespace clean_test::framework {

Registry & registry() noexcept
{
    static auto data = Registry{};
    return data;
}

}
