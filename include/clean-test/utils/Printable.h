// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <iosfwd>
#include <utility>

namespace clean_test::utils {

/// Detects whether @tparam T can be printed to @c std::ostream s.
template <typename T>
concept Printable = requires(std::ostream & out, T t) {
    { out << t } -> std::same_as<std::ostream &>;
};

}
