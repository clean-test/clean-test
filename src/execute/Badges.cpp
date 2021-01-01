// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "Badges.h"

#include <array>

namespace clean_test::execute {
namespace {

using namespace std::string_view_literals;

constexpr auto badges = std::array{
    "[       ]"sv,
    "[ ===== ]"sv,
    "[ ----- ]"sv,
    "[ RUN   ]"sv,
    "[ PASS  ]"sv,
    "[ FAIL  ]"sv,
    "[ ABORT ]"sv,
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Badge badge(BadgeType const type)
{
    return badges[static_cast<std::size_t>(type)];
}

}
