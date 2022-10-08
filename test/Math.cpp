// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <clean-test/utils/Math.h>

#include <array>
#include <vector>

namespace {

namespace ct = clean_test;
using ct::utils::dynamic_assert;

template <typename T>
class Wrapper {
public:
    auto operator<=>(Wrapper const & other) const noexcept = default;

    constexpr auto operator-() const
    {
        if constexpr (std::is_signed_v<std::remove_cvref_t<T>>) {
            return Wrapper{static_cast<T>(-m_value)};
        } else {
            return Wrapper{m_value};
        }
    }

    T m_value = {};
};

template <typename T>
Wrapper(T&) -> Wrapper<T>;

template <typename T>
class StaticallyMockable {
public:
    static T m_norm;

    auto operator<=>(StaticallyMockable const & other) const noexcept = default;
};

template <typename T>
T StaticallyMockable<T>::m_norm = {};

template <typename T>
constexpr auto norm(StaticallyMockable<T>)
{
    return StaticallyMockable<T>::m_norm;
}

template <typename T>
void test_floaing_point()
{
    using Limits = std::numeric_limits<T>;
    using Mock = StaticallyMockable<T>;

    static_assert(std::is_same_v<T, std::remove_cvref_t<decltype(ct::utils::norm(T{}))>>);
    static_assert(std::is_same_v<T, decltype(std::abs(T{}))>);
    static_assert(std::is_same_v<Wrapper<T>, std::remove_cvref_t<decltype(ct::utils::norm(Wrapper<T>{}))>>);
    static_assert(std::is_same_v<T, decltype(ct::utils::norm(Mock{}))>);

    auto const values = std::vector<T>{
        0.f, -0.f, 1.f, -1.f, Limits::infinity(), -Limits::infinity(), Limits::max(), Limits::lowest()};
    for (auto const f : values) {
        auto const abs = std::fabs(f);
        dynamic_assert(ct::utils::norm(f) == abs);
        dynamic_assert(ct::utils::norm(Wrapper{f}).m_value == abs);

        Mock::m_norm = f;
        dynamic_assert(ct::utils::norm(Mock{}) == f);
    }

    // Test that the fallback implementation works with a quiet NaN.
    auto const nan = Limits::quiet_NaN();
    dynamic_assert(std::isnan(ct::utils::norm(Wrapper<T>{nan}).m_value));
}

template <typename T>
void test_integral()
{
    auto const abs = [](T t) {
        if constexpr (std::is_signed_v<T>) {
            return static_cast<T>(std::abs(t));
        } else {
            return t;
        }
    };

    using Limits = std::numeric_limits<T>;
    using Mock = StaticallyMockable<T>;

    static_assert(std::is_same_v<T, std::remove_cvref_t<decltype(ct::utils::norm(T{}))>>);
    static_assert(std::is_same_v<Wrapper<T>, std::remove_cvref_t<decltype(ct::utils::norm(Wrapper<T>{}))>>);
    static_assert(std::is_same_v<T, decltype(ct::utils::norm(Mock{}))>);

    auto const values = std::vector<T>{0, 1, 2, Limits::max()};
    auto const signs = std::array{std::vector<int>{1}, std::vector<int>{1, -1}};
    for (auto const v : values) {
        for (auto const sign : signs[std::is_signed_v<T>]) {
            auto const value = static_cast<T>(sign * v);
            dynamic_assert(ct::utils::norm(value) == abs(value));
            dynamic_assert(ct::utils::norm(Wrapper{value}).m_value == abs(value));

            Mock::m_norm = value;
            dynamic_assert(ct::utils::norm(Mock{}) == value);
        }
    }
}

/// Checks that T has a @c std::abs which is constexpr (which it is with C++23).
///
/// Also checks that @tparam T is constexpr default constructible. We accept this since @c std::abs is only ever
/// provided for builtin types which automatically are constexpr anyways.
template <typename T>
concept HasConstexprStdAbsSupport = requires {
    { std::bool_constant<(std::abs(std::remove_cvref_t<T>{}), true)>{} } -> std::same_as<std::true_type>;
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static_assert(ct::utils::math_details::HasStdAbsSupport<int>);
static_assert(ct::utils::math_details::HasStdAbsSupport<const int&>);
static_assert(ct::utils::math_details::HasStdAbsSupport<int&>);
static_assert(ct::utils::math_details::HasStdAbsSupport<float>);
static_assert(not ct::utils::math_details::HasStdAbsSupport<char>);

static_assert(ct::utils::math_details::ManuallyNormable<int>);
static_assert(ct::utils::math_details::ManuallyNormable<float &>);
static_assert(ct::utils::math_details::ManuallyNormable<const char&>);
static_assert(ct::utils::math_details::ManuallyNormable<Wrapper<int>>);
static_assert(not ct::utils::math_details::ManuallyNormable<std::string_view>);

static_assert(ct::utils::math_details::Normable<float>);
static_assert(ct::utils::math_details::Normable<int &>);
static_assert(ct::utils::math_details::Normable<const char&>);
static_assert(not ct::utils::math_details::Normable<std::string_view>);

static_assert(not HasConstexprStdAbsSupport<float> or ct::utils::norm(-0.f) == 0.f);
static_assert(ct::utils::norm(1u) == 1u);
static_assert(not HasConstexprStdAbsSupport<int> or ct::utils::norm(-2) == 2);

int main()
{
    test_floaing_point<float>();
    test_floaing_point<double>();
    test_floaing_point<long double>();

    test_integral<int>();
    test_integral<long>();
    test_integral<long long>();
    test_integral<char>();

    test_integral<unsigned char>();
    test_integral<unsigned int>();
    test_integral<unsigned long>();
}
