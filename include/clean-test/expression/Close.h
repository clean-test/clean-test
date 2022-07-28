// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Distance.h"
#include "Tolerance.h"

namespace clean_test::expression {

/// Shorthand to test that @c distance(lhs, rhs) is at most the default tolerance.
///
/// Adjusting the tolerance can be achieved by spelling out the check manually instead, e.g.
///
///     distance(lhs, rhs) <= tolerance(whatever)
template <typename L, typename R>
constexpr auto is_close(L && lhs, R && rhs)
{
    auto dist = distance(std::forward<L>(lhs), std::forward<R>(rhs));
    return std::move(dist) <= tolerance<typename decltype(dist)::NormValue>();
}

}
