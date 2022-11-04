// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Base.h"

#include <clean-test/utils/Tuple.h>

#include <ostream>
#include <string_view>
#include <type_traits>

namespace clean_test::expression {

/// Wrapper for a value with a description.
template <typename T>
class DescribedValue {
public:
    T m_value;
    std::string_view m_description;
};

namespace lazy_detector_details {

/// Value-based implementation for @c Detection.
///
/// This is the general template; there is also a specialization for @c DescribedValue below.
template <typename T>
class DetectionDetails {
public:
    using Value = T;

    constexpr explicit DetectionDetails(std::convertible_to<Value> auto && value) :
        m_value{std::forward<decltype(value)>(value)}
    {}

    constexpr Value const & value() const {
        return m_value;
    }

    friend std::ostream & operator<<(std::ostream & out, DetectionDetails const & details) {
        return out << details.value();
    }

private:
    Value m_value;
};

/// Implementation of @c Detection based on @c DescribedValue.
template <typename T>
class DetectionDetails<DescribedValue<T>>
{
public:
    using Value = T;

    constexpr explicit DetectionDetails(std::convertible_to<DescribedValue<T>> auto && data) :
        m_data{std::forward<decltype(data)>(data)}
    {}

    constexpr Value const & value() const {
        return m_data.m_value;
    }

    friend std::ostream & operator<<(std::ostream & out, DetectionDetails const & details) {
        return out << details.m_data.m_description;
    }

private:
    DescribedValue<T> m_data;
};

}

template <typename Details> requires(only_values<Details>)
class Detection;

/// Basic detection clause
///
/// Uses @tparam Detector in determine a binary predicate for some @tparam Func object. This detection happens lazily
/// i.e. upon first query of the predicate.
///
/// There are two different cases how the detection result is handled:
///  - Either it is used as the value directly, e.g. in @c lift where users control the callable.
///  - Or (for @c DescribedValue s) we handle value / description separately. This is only used for clean-test internal
///    detectors (e.g. abort and throw).
template <typename Detector, std::invocable<> Func>
class LazyDetector : public ExpressionBase<LazyDetector<Detector, Func>>, private Detector {
    using Result = std::remove_cvref_t<std::invoke_result_t<Detector, Func>>;
    using Details = lazy_detector_details::DetectionDetails<Result>;
public:
    using Value = typename Details::Value;
    using Evaluation = Detection<Details>;

    /// Detailed c'tor: Capture @p func for (potential) later detection.
    constexpr explicit LazyDetector(std::convertible_to<Func> auto && func) : m_func{std::forward<decltype(func)>(func)}
    {}

    [[nodiscard]] constexpr Evaluation evaluation() const
    {
        return Evaluation{Details{Detector::operator()(m_func)}};
    }

private:
    Func m_func; //!< Object for next predicate detection (if any).
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Cached detected criterion
template <typename Details> requires(only_values<Details>)
class Detection : public Details, public EvaluationBase<Detection<Details>> {
public:
    constexpr explicit Detection(std::convertible_to<Details> auto && details) :
        Details{std::forward<decltype(details)>(details)}
    {}
};

}
