// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "LazyDetector.h"

#include <concepts>
#if __has_include(<unistd.h>) and __has_include(<sys/wait.h>)
# include <sys/wait.h>
# include <unistd.h>
#endif

namespace clean_test::expression {

/// Helper for detecting process aborts.
class DetectAbort {
public:
    /// Determine whether invocation of @p func aborts (tested in a fork).
    template <std::invocable<> Func>
    bool operator()(Func && func) const;
};

/// Helper for detecting process aborts in debug mode.
class DetectAbortDebug : private DetectAbort {
public:
    /// Determine whether invocation of @p func aborts iff in debug mode.
    template <std::invocable<> Func>
    bool operator()(Func && func) const;
};

/// Make lazy detection expression to identify whether invoking @p func aborts the process.
template <std::invocable<> Func>
auto aborts(Func && func)
{
    return LazyDetector<DetectAbort, Func>{std::forward<Func>(func)};
}

/// Make lazy detection expression to identify whether invoking @p func aborts the process (in debug mode).
template <std::invocable<> Func>
auto debug_aborts(Func && func)
{
    return LazyDetector<DetectAbortDebug, Func>{std::forward<Func>(func)};
}

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <std::invocable<> Func>
bool DetectAbort::operator()(Func && func) const
{
    // Fork process (so we don't die)
    if (auto const child = fork(); child == 0) {
        // Try invoking func in child.
        std::forward<Func>(func)();
        std::exit(0);
    }
    // ... and evaluate exit-code in parent.
    auto status = 0;
    wait(&status);
    return status;
}

template <std::invocable<> Func>
bool DetectAbortDebug::operator()(Func && func) const
{
    constexpr auto is_debug =
#ifdef NDEBUG
        false
#else
        true
#endif
    ;
    return DetectAbort::operator()(std::forward<Func>(func)) == is_debug;
}

}
