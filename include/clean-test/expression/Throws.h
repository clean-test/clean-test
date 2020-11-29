// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "LazyDetector.h"

#include <concepts>

namespace clean_test::expression {

/// Helper for detecting any exception.
class DetectCatchAny {
public:
    /// Determine whether invoking @p func throws (anything).
    template <std::invocable<> Func>
    bool operator()(Func && func) const;
};

/// Helper for detecting exception of type @tparam Xcp.
template <typename Xcp>
class DetectCatch {
public:
    /// Determine whether invoking @p func throws an instance of type @tparam Xcp.
    template <std::invocable<> Func>
    bool operator()(Func && func) const;
};


/// Make lazy detection expression to identify whether invoking @p func throws any exception.
template <std::invocable<> Func>
constexpr auto throws(Func && func)
{
    return LazyDetector<DetectCatchAny, Func>{std::forward<Func>(func)};
}

/// Make lazy detection expression to identify whether invoking @p func throws @tparam Xcp.
template <typename Xcp, std::invocable<> Func>
constexpr auto throws(Func && func)
{
    return LazyDetector<DetectCatch<Xcp>, Func>{std::forward<Func>(func)};
}

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::invocable<> Func>
bool DetectCatchAny::operator()(Func && func) const
{
    try {
        std::forward<Func>(func)();
    } catch (...) {
        return true;
    }
    return false;
}

template <typename Xcp>
template <std::invocable<> Func>
bool DetectCatch<Xcp>::operator()(Func && func) const
{
    try {
        std::forward<Func>(func)();
    } catch (Xcp const &) {
        return true;
    } catch (...) {
    }
    return false;
}

}
