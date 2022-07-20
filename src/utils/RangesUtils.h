// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <ranges>
#include <version>

namespace clean_test::utils {

/// Variant of @c std::count_if for whole containers.
template <typename Data, std::invocable<typename Data::value_type> Predicate>
constexpr std::size_t count_if(Data const & data, Predicate && predicate)
{
#if defined(__cpp_lib_ranges) and __cpp_lib_ranges >= 201900
    return std::ranges::count_if(data, std::forward<Predicate>(predicate));
#else
    return std::count_if(std::cbegin(data), std::cend(data), std::forward<Predicate>(predicate));
#endif
}


}
