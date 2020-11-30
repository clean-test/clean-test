// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <utils/ScopeGuard.h>

#include <chrono>
#include <concepts>
#include <ostream>
#include <cmath>

namespace clean_test::utils {

/// Convenience shorthand for printing a duration with appropriate unit to achieve a float-representation >= 1.f.
template <typename Duration>
class WithAdaptiveUnit {
public:
    /// Detailed c'tor: Initialize from @p duration to print.
    explicit WithAdaptiveUnit(Duration const & duration)
        : m_duration{duration}
    {}

    /// Stream @p wau into @p out.
    friend std::ostream & operator<<(std::ostream & out, WithAdaptiveUnit const & wau)
    {
        wau.print_to(out);
        return out;
    }

private:
    template <typename D>
    using As = std::chrono::duration<float, typename D::period>;

    void print_to(std::ostream & out) const;

    template <typename D>
    bool try_print_as(std::ostream & out, std::string_view const unit) const;

    template <typename D>
    static void print_duration(std::ostream & out, D && d, std::string_view const unit);

    Duration const & m_duration; //!< The duration to be printed.
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Duration>
void WithAdaptiveUnit<Duration>::print_to(std::ostream & out) const
{
    if (try_print_as<std::chrono::hours>(out, "h")) {
        return;
    }
    if (try_print_as<std::chrono::minutes>(out, "m")) {
        return;
    }
    if (try_print_as<std::chrono::seconds>(out, "s")) {
        return;
    }
    if (try_print_as<std::chrono::milliseconds>(out, "ms")) {
        return;
    }
    if (try_print_as<std::chrono::microseconds>(out, "us")) {
        return;
    }
    print_duration(out, As<std::chrono::nanoseconds>{m_duration}, "ns");
}

template <typename Duration>
template <typename D>
bool WithAdaptiveUnit<Duration>::try_print_as(std::ostream & out, std::string_view const unit) const
{
    auto const value = As<D>{m_duration};
    auto const result = (value.count() >= 1.f);
    if (result) {
        print_duration(out, value, unit);
    }
    return result;
}

template <typename Duration>
template <typename D>
void WithAdaptiveUnit<Duration>::print_duration(std::ostream & out, D && d, std::string_view const unit)
{
    static constexpr auto num_places = [](float const num) constexpr
    {
        return static_cast<int>(std::floor(std::log10(num))) + 1;
    };

    auto const reset = ScopeGuard{[&, flags = out.flags(), precision = out.precision()]{
        out.flags(flags);
        out.precision(precision);
    }};

    // Shift decimal point (adjust precision) s.t. ordinary durations (< 1k h) are printed with width 5 e.g. "w.xyz".
    out.precision(std::max(4 - num_places(d.count()), 1));
    out << std::fixed << d.count() << unit;
}

}
