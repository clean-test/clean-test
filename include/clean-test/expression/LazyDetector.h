// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <optional>
#include <iosfwd>

namespace clean_test::expression {

/// Basic detection claus
///
/// Uses @tparam Detector in determine a binary predicate for some @tparam Func object. This detection happens lazily
/// i.e. upon first query of the predicate.
template <typename Detector, std::invocable<> Func>
class LazyDetector : public Base, private Detector {
public:
    /// Detailed c'tor: Capture @p func for (potential) later detection.
    constexpr explicit LazyDetector(std::convertible_to<Func> auto && func) : m_func{std::forward<decltype(func)>(func)}
    {}

    /// Determine (and cache) whether invoking @c m_func throws (according to @tparam Detector).
    [[nodiscard]] constexpr explicit operator bool() const;

    /// Output detection result (if any) of @p self into @p out.
    friend std::ostream & operator<<(std::ostream & out, LazyDetector const & self)
    {
        return self.print_to(out);
    }

private:
    std::ostream & print_to(std::ostream & out) const;

    mutable std::optional<Func> m_func;      //!< Object for next predicate detection (if any).
    mutable bool m_detection_result = false; //!< Detected binary predicate.
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Detector, std::invocable<> Func>
constexpr LazyDetector<Detector, Func>::operator bool() const
{
    if (m_func) {
        m_detection_result = Detector::operator()(std::move(*m_func));
        m_func.reset();
    }
    return m_detection_result;
}

template <typename Detector, std::invocable<> Func>
std::ostream & LazyDetector<Detector, Func>::print_to(std::ostream & out) const
{
    if (m_func) {
        out << "unevaluated";
    } else {
        out << m_detection_result;
    }
    return out;
}

}
