// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <ostream>

namespace clean_test::expression {

class Detection;

/// Basic detection clause
///
/// Uses @tparam Detector in determine a binary predicate for some @tparam Func object. This detection happens lazily
/// i.e. upon first query of the predicate.
template <typename Detector, std::invocable<> Func>
class LazyDetector : public ExpressionBase<LazyDetector<Detector, Func>>, private Detector {
public:
    using Value = bool;
    using Evaluation = Detection;

    /// Detailed c'tor: Capture @p func for (potential) later detection.
    constexpr explicit LazyDetector(std::convertible_to<Func> auto && func) : m_func{std::forward<decltype(func)>(func)}
    {}

    [[nodiscard]] constexpr Detection evaluation() const;

private:
    Func const & m_func; //!< Object for next predicate detection (if any).
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Cached detected criterion
class Detection : public EvaluationBase<Detection> {
public:
    constexpr explicit Detection(bool const detection_result) : m_detection_result{detection_result}
    {}

    [[nodiscard]] constexpr bool value() const
    {
        return m_detection_result;
    }

    friend std::ostream & operator<<(std::ostream & out, Detection const & detection)
    {
        return out << detection.m_detection_result;
    }

private:
    bool m_detection_result;
};

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Detector, std::invocable<> Func>
constexpr Detection LazyDetector<Detector, Func>::evaluation() const
{
    return Detection{Detector::operator()(m_func)};
}

}
