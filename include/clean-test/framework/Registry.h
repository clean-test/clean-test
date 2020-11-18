// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Case.h"

#include <vector>

namespace clean_test::framework {

using Registry = std::vector<Case>;

/// Access statically managed set of all registered test-cases.
Registry & registry() noexcept;

}
