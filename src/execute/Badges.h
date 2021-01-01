// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <string_view>

namespace clean_test::execute {

/// A badge is a prepared prefix for the console output.
using Badge = std::string_view;

/// All kinds of different, supported badges
enum class BadgeType {
    empty,
    title,
    headline,
    start,
    pass,
    fail,
    abort,
};

/// Access / generato badge for given @p type.
Badge badge(BadgeType type);

}
