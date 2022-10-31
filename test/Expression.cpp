// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <clean-test/expression.h>
#include <clean-test/utils/SourceLocation.h>

#include <cassert>
#include <exception>
#include <sstream>
#include <vector>

namespace {

namespace ct = clean_test;

template <typename T>
void assert_output(
    std::string_view const assertion,
    T const & value,
    ct::utils::SourceLocation const & where = ct::utils::SourceLocation::current())
{
    auto buffer = std::ostringstream{};
    buffer << value;
    auto const generated = buffer.str();
    if (assertion != generated) {
        std::cerr << "Wrong output: \"" << assertion << "\" != \"" << generated << "\" (" << where.file_name() << ":"
                  << where.line() << ')' << std::endl;
        std::terminate();
    }
}

class ConversionTracker {
public:
    constexpr ConversionTracker(bool const value) : m_value{value}
    {}

    ConversionTracker(ConversionTracker const &) = delete;

    explicit operator bool() const
    {
        m_was_converted = true;
        return m_value;
    }

    friend std::ostream & operator<<(std::ostream & out, ConversionTracker const & data)
    {
        return out << data.m_value;
    }

    bool was_converted() const
    {
        return m_was_converted;
    }

private:
    mutable bool m_was_converted = false;
    bool m_value;
};

/// Integer wrapper whose comma operator won't warn for discarded first parameter.
class CommaInt {
public:
    constexpr explicit(false) CommaInt(int v) noexcept : m_v{v}
    {}

    explicit(false) constexpr operator int() const noexcept
    {
        return m_v;
    }

    friend constexpr decltype(auto) operator,([[maybe_unused]] CommaInt const &, auto && v) noexcept
    {
        return std::forward<decltype(v)>(v);
    }

    int m_v;
};

template <typename T>
struct PtrWrap {
    constexpr auto operator *() const {
        return *ptr;
    }
    T * ptr;
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// constexpr tests

// truth
static_assert(ct::lift(true));
static_assert(not ct::lift(false));

// BasicExpression concept invariants
static_assert(not ct::expression::BasicExpression<ConversionTracker>);
static_assert(not ct::expression::BasicExpression<bool>);
static_assert(ct::expression::BasicExpression<ct::expression::Clause<bool> const &>);
static_assert(ct::expression::BasicExpression<ct::expression::Clause<bool>>);
static_assert(ct::expression::BasicExpression<decltype(ct::lift(true))>);
static_assert(ct::expression::BasicExpression<decltype(ct::lift(ct::expression::Clause{false}))>);

// unary not
static_assert(not ct::lift(false));
static_assert(not not true);
static_assert(not (not ct::lift(true)));

// Equality comparison
static_assert(17 == ct::lift(17));
static_assert(not (17 == ct::lift(42)));
static_assert(ct::lift(true) == (ct::lift(true) == ct::lift(true)));

// binary and
static_assert(ct::lift(true) and ct::lift(true));
static_assert(not (true and ct::lift(false)));
static_assert(not (ct::lift(false) and true));
static_assert(not (ct::lift(false) and ct::lift(false)));
static_assert(std::string_view{"asdf"} == ct::lift(std::string_view{"asdf"}));
static_assert(not static_cast<bool>(std::string_view{"asdf"} == ct::lift(std::string_view{"not asdf"})));

static_assert(ct::lift(1) + 1);
static_assert(ct::lift(2) - 1);
static_assert(ct::lift(1) * 1);
static_assert(ct::lift(1) / 1);
static_assert(+ct::lift(1));
static_assert(-ct::lift(1));
static_assert(ct::lift(1) % 2);

static_assert(ct::lift(1) == 1);
static_assert(ct::lift(1) != 2);
static_assert(ct::lift(2) > 1);
static_assert(ct::lift(1) < 2);
static_assert(ct::lift(2) >= 1);
static_assert(ct::lift(1) <= 2);

static_assert(ct::lift(1) & 1);
static_assert(ct::lift(2) | 1);
static_assert(ct::lift(2) ^ 1);
static_assert(~ct::lift(1));
static_assert((ct::lift(CommaInt{2}), ct::lift(1)));
static_assert(ct::lift(0) or 1);

template <typename L, typename R>
concept Addable = requires(L const & l, R const & r) { {l + r}; };
static_assert(Addable<int, ct::expression::Lift<int>>);
static_assert(not Addable<int, ct::expression::Lift<std::string_view>>);

void test_operator_output()
{
    assert_output("( 1 + 2 )", 1 + ct::lift(2));
    assert_output("( 3 - 2 )", 3 - ct::lift(2));
    assert_output("( 2 * 3 )", 2 * ct::lift(3));
    assert_output("( 3 / 2 )", 3 / ct::lift(2));
    assert_output("+3", +ct::lift(3));
    assert_output("-2", -ct::lift(2));
    assert_output("( 3 % 2 )", 3 % ct::lift(2));

    assert_output("( 1 == 1 )", 1 == ct::lift(1));
    assert_output("( 1 != 2 )", 1 != ct::lift(2));
    assert_output("( 2 > 1 )", 2 > ct::lift(1));
    assert_output("( 1 < 2 )", 1 < ct::lift(2));
    assert_output("( 2 >= 1 )", 2 >= ct::lift(1));
    assert_output("( 1 <= 2 )", 1 <= ct::lift(2));

    assert_output("( 1 & 1 )", 1 & ct::lift(1));
    assert_output("( 1 | 2 )", 1 | ct::lift(2));
    assert_output("( 1 ^ 2 )", 1 ^ ct::lift(2));
    assert_output("~2", ~ct::lift(2));

    constexpr auto one = 1;
    auto const expected = [&] {
        auto buffer = std::ostringstream{};
        buffer << one << " (@" << &one << ')';
        return std::move(buffer).str();
    }();
    assert_output(expected, *ct::lift(&one));
    auto const wrapped_one = PtrWrap<int const>{&one};
    assert_output("1", *ct::lift(wrapped_one));

    assert_output("2, 3", (ct::lift(CommaInt{2}), ct::lift(3)));
}

void test_literals()
{
    using namespace ct::literals;
    static_assert(1_i != 2_i);
    static_assert(0_ull == 0);
    static_assert(1_z == 1);
    static_assert(1_uz == 1u);
    static_assert(-1_t == -1);
    static_assert(0_ut != -1);
    static_assert(3.7 != 3.8_f);
    static_assert("hi"_sv != "how are you");
}

void test_short_circuit_and()
{
    // single layer of short circuiting: (t1 and t2)
    {
        auto const t0 = ConversionTracker{false};
        auto const t1 = ConversionTracker{true};

        auto const conjunction_base = (ct::lift(t0) and t1);
        auto const conjunction = conjunction_base.evaluation();
        ct::utils::dynamic_assert(not conjunction);
        ct::utils::dynamic_assert(t0.was_converted());
        ct::utils::dynamic_assert(not t1.was_converted()); // due to short circuiting
        assert_output("( 0 and <unknown> )", conjunction);
    }

    // double layer of short circuiting: ((t0 and t1) and (t2 and t3))
    {
        auto const t0 = ConversionTracker{false};
        auto const t1 = ConversionTracker{true};
        auto const t2 = ConversionTracker{true};
        auto const t3 = ConversionTracker{true};

        auto const conjunction_base = ((ct::lift(t0) and t1) and (t2 and ct::lift(t3)));
        auto const conjunction = conjunction_base.evaluation();
        ct::utils::dynamic_assert(not conjunction);
        ct::utils::dynamic_assert(t0.was_converted());
        ct::utils::dynamic_assert(not t1.was_converted()); // due to short circuiting
        ct::utils::dynamic_assert(not t2.was_converted()); // due to short circuiting
        ct::utils::dynamic_assert(not t3.was_converted()); // due to short circuiting
        // notably the right and layer isn't even expanded a single stage.
        assert_output("( ( 0 and <unknown> ) and <unknown> )", conjunction);
    }
}

void test_short_circuit_or()
{
    // single layer of short circuiting: (t1 or t2)
    {
        auto const t0 = ConversionTracker{true};
        auto const t1 = ConversionTracker{false};

        auto const disjunction_base = (ct::lift(t0) or t1);
        auto const disjunction = disjunction_base.evaluation();
        ct::utils::dynamic_assert(disjunction);
        ct::utils::dynamic_assert(t0.was_converted());
        ct::utils::dynamic_assert(not t1.was_converted()); // due to short circuiting
        assert_output("( 1 or <unknown> )", disjunction);
    }

    // double layer of short circuiting: ((t0 and t1) and (t2 and t3))
    {
        auto const t0 = ConversionTracker{true};
        auto const t1 = ConversionTracker{false};
        auto const t2 = ConversionTracker{false};
        auto const t3 = ConversionTracker{false};

        auto const disjunction_base = ((ct::lift(t0) or t1) or (t2 or ct::lift(t3)));
        auto const disjunction = disjunction_base.evaluation();
        ct::utils::dynamic_assert(disjunction);
        ct::utils::dynamic_assert(t0.was_converted());
        ct::utils::dynamic_assert(not t1.was_converted()); // due to short circuiting
        ct::utils::dynamic_assert(not t2.was_converted()); // due to short circuiting
        ct::utils::dynamic_assert(not t3.was_converted()); // due to short circuiting
        // notably the right and layer isn't even expanded a single stage.
        assert_output("( ( 1 or <unknown> ) or <unknown> )", disjunction);
    }
}

template <typename T, typename U = T>
concept Identifiable = requires(T t, U u) {
    { t == u } -> std::convertible_to<bool>;
    { t != u } -> std::convertible_to<bool>;
    { u == t } -> std::convertible_to<bool>;
    { u != t } -> std::convertible_to<bool>;
};

template <typename T, typename U = T>
concept Ordered = requires(T t, U u) {
    { t < u } -> std::convertible_to<bool>;
    { t <= u } -> std::convertible_to<bool>;
    { t > u } -> std::convertible_to<bool>;
    { t >= u } -> std::convertible_to<bool>;
    { u < t } -> std::convertible_to<bool>;
    { u <= t } -> std::convertible_to<bool>;
    { u > t } -> std::convertible_to<bool>;
    { u >= t } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept OrderedOnly
    = Ordered<T, U> and not Ordered<T, T> and not Ordered<U, U>
    and not Identifiable<T, U> and not Identifiable<T, T> and not Identifiable<U, U>;

template <typename T>
class StaticallyMockable {
public:
    static std::vector<T> m_norm_stack;

    auto operator<=>(StaticallyMockable const & other) const noexcept = delete;

    friend auto operator-(StaticallyMockable, StaticallyMockable) {
        return StaticallyMockable{};
    }

    friend std::ostream & operator<<(std::ostream & out, StaticallyMockable) {
        return out << "StaticallyMockable";
    }
};

template <typename T>
std::vector<T> StaticallyMockable<T>::m_norm_stack = {};

template <typename T>
constexpr auto norm(StaticallyMockable<T>)
{
    auto & data = StaticallyMockable<T>::m_norm_stack;
    ct::utils::dynamic_assert(not data.empty());
    auto result = std::move(data.back());
    data.pop_back();
    return result;
}

void test_distance() {
    // compile time checks on unsigned, since this is the only constexpr-norm available for both MSVC and clang.
    static_assert(ct::distance(4u, 1u) <= ct::tolerance(3u, 0u));
    static_assert(not ct::is_close(1u, 2u));
    static_assert(ct::is_close(ct::lift(1u) + 3u, ct::lift(6u) - 2u));

    static_assert(OrderedOnly<ct::expression::Distance<int>, ct::expression::Tolerance<int>>);
    static_assert(not OrderedOnly<ct::expression::Distance<int>, ct::expression::Tolerance<double>>);
    static_assert(not Ordered<ct::expression::Distance<int>>);
    static_assert(not Identifiable<ct::expression::Distance<int>>);
    static_assert(not Ordered<ct::expression::Tolerance<int>>);
    static_assert(not Identifiable<ct::expression::Tolerance<int>>);

    // Distance / Tolerance comparison operator.
    using D = ct::expression::Distance<int>;
    using T = ct::expression::Tolerance<int>;
    auto as_tolerance = [](D const & dist) {
        return T{.m_absolute = dist.m_absolute, .m_relative = dist.m_relative};
    };
    auto compare_all = [&](auto const & positive, auto const & negative, D const & lhs, D const & rhs, bool const expectation) {
        // note: lhs < T{rhs} is not equivalent to not (lhs >= T{rhs}).
        ct::utils::dynamic_assert(expectation == positive(lhs, as_tolerance(rhs)));
        ct::utils::dynamic_assert(expectation == negative(as_tolerance(rhs), lhs));
        ct::utils::dynamic_assert(expectation == positive(as_tolerance(lhs), rhs));
        ct::utils::dynamic_assert(expectation == negative(rhs, as_tolerance(lhs)));
    };
    auto test_less_equal = [&](D const & lhs, D const & rhs) {
        compare_all(std::less_equal<>{}, std::greater_equal<>{}, lhs, rhs, true);
    };
    auto test_less = [&](D const & lhs, D const & rhs) {
        test_less_equal(lhs, rhs);
        compare_all(std::less<>{}, std::greater<>{}, lhs, rhs, true);
    };
    auto test_not_less = [&](D const & lhs, D const & rhs) {
        compare_all(std::less<>{}, std::greater<>{}, lhs, rhs, false);
    };
    auto test_not_less_equal = [&](D const & lhs, D const & rhs) {
        test_not_less(lhs, rhs);
        compare_all(std::less_equal<>{}, std::greater_equal<>{}, lhs, rhs, false);
    };

    auto const north = D{0, 1};
    auto const south = D{0, -1};
    auto const east = D{1, 0};
    auto const west = D{-1, 0};

    test_less(west, north);
    test_less(east, north);
    test_less(north, east);
    test_less(south, north);
    test_less(south, west);
    test_less(west, south);
    test_less(south, east);

    test_less_equal(north, south);
    test_less_equal(south, north);
    test_less_equal(north, north);
    test_less_equal(south, south);
    test_less_equal(east, east);
    test_less_equal(west, west);
    test_less_equal(south, south);
    test_less_equal(north, north);

    test_not_less_equal(north, west);
    test_not_less_equal(east, south);

    test_not_less(north, south);
    test_not_less(east, east);
    test_not_less(west, west);
    test_not_less(south, south);
    test_not_less(north, north);

    // With a type T where norm(T) != T
    auto & data = StaticallyMockable<int>::m_norm_stack;
    data.push_back(1);
    data.push_back(2);
    data.push_back(3);
    assert_output(
        "( distance(StaticallyMockable, StaticallyMockable) = {absolute: 3, relative: 1} "
        ">= {absolute: 1, relative: 100} )",
        ct::distance(StaticallyMockable<int>{}, StaticallyMockable<int>{}) >= ct::tolerance(1, 100));

    // with references
    constexpr auto x = ct::lift(0.15) + 0.15;
    constexpr auto y = ct::lift(0.1) + 0.2;
    constexpr auto eps = x / 10.;
    ct::utils::dynamic_assert(ct::distance(x, y) <= ct::tolerance(eps, eps));

    // Operator console output
    assert_output(
        "( distance(( 1 + 0.3 ), ( 2 - 0.7 )) = {absolute: 0, relative: 0} "
        "<= {absolute: 2.22045e-16, relative: 2.22045e-16} )",
        ct::is_close(ct::lift(1.) + .3, ct::lift(2) - .7));
}

int main()
{
    test_operator_output();
    test_literals();
    test_short_circuit_and();
    test_short_circuit_or();
    test_distance();

    // Abortion
#ifdef CLEANTEST_HAS_ABORT_SUPPORT
    ct::utils::dynamic_assert(ct::aborts([] { std::abort(); }));
    ct::utils::dynamic_assert(ct::aborts([] { std::terminate(); }));
    ct::utils::dynamic_assert(not ct::aborts([] { return 42; }));
    ct::utils::dynamic_assert(ct::debug_aborts([] { assert(false); }));
#endif

    // Exceptions
    ct::utils::dynamic_assert(ct::throws([] { throw 42; }));
    ct::utils::dynamic_assert(not ct::throws([] { return 42; }));
    ct::utils::dynamic_assert(ct::throws<std::exception>([] { throw std::runtime_error{"violation"}; }));
    ct::utils::dynamic_assert(not ct::throws<std::exception>([] { throw 42; }));
    ct::utils::dynamic_assert(not ct::throws<std::exception>([] { return 42; }));
}
