// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "LazyDetector.h"

#include <concepts>
#if __has_include(<unistd.h>) and __has_include(<sys/wait.h>)
# define CLEANTEST_HAS_ABORT_SUPPORT unix
# include <string_view>

# include <sys/wait.h>
# include <unistd.h>

namespace clean_test::expression {

/// Helper for detecting process aborts.
class DetectAbort {
public:
    /// Determine whether invocation of @p func aborts (tested in a fork).
    template <std::invocable<> Func>
    DescribedValue<bool> operator()(Func && func) const;
};

/// Helper for detecting process aborts in debug mode.
class DetectAbortDebug : private DetectAbort {
public:
    /// Determine whether invocation of @p func aborts iff in debug mode.
    template <std::invocable<> Func>
    DescribedValue<bool> operator()(Func && func) const;
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
DescribedValue<bool> DetectAbort::operator()(Func && func) const
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
    auto const abort_detected = (status != 0);
    return {abort_detected, abort_detected ? "aborts" : "no-abort"};
}

template <std::invocable<> Func>
DescribedValue<bool> DetectAbortDebug::operator()(Func && func) const
{
    constexpr auto is_debug =
#ifdef NDEBUG
        false
#else
        true
#endif
    ;
    static std::string_view const descriptions[] = {
        "abort-avoided-optimized", "incorrect-abort-optimized", "no-abort-debug", "aborts-debug"};
    auto const abort_detected = DetectAbort::operator()(std::forward<Func>(func)).m_value;
    return {abort_detected == is_debug, descriptions[is_debug * 2 + abort_detected]};
}

}
#endif
