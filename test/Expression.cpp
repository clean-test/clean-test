// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <clean-test/utils/SourceLocation.h>

#include <cassert>
#include <clean-test/expression.h>
#include <exception>
#include <iostream>
#include <sstream>

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

static_assert((ct::lift(2), ct::lift(1)));

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

    assert_output("2, 3", (ct::lift(2), ct::lift(3)));
}

void test_literals()
{
    using namespace ct::literals;
    static_assert(1_i != 2_i);
    static_assert(0_ull == 0);
    static_assert(3.7 != 3.8_f);
    static_assert("hi"_sv != "how are you");
}

void test_short_circuit_and()
{
    // single layer of short circuiting: (t1 and t2)
    {
        auto const t0 = ConversionTracker{false};
        auto const t1 = ConversionTracker{true};
        auto const c0 = ct::lift(t0);
        auto const c1 = ct::lift(t1);

        auto const conjunction_base = (c0 and c1);
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
        auto const c0 = ct::lift(t0);
        auto const c1 = ct::lift(t1);
        auto const c2 = ct::lift(t2);
        auto const c3 = ct::lift(t3);

        auto const conjunction_left = (c0 and c1);
        auto const conjunction_right = (c2 and c3);
        auto const conjunction_base = (conjunction_left and conjunction_right);
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

int main()
{
    test_operator_output();
    test_literals();
    test_short_circuit_and();

    // Abortion
    ct::utils::dynamic_assert(ct::aborts([] { std::abort(); }));
    ct::utils::dynamic_assert(ct::aborts([] { std::terminate(); }));
    ct::utils::dynamic_assert(not ct::aborts([] { return 42; }));
    ct::utils::dynamic_assert(ct::debug_aborts([] { assert(false); }));

    // Exceptions
    ct::utils::dynamic_assert(ct::throws([] { throw 42; }));
    ct::utils::dynamic_assert(not ct::throws([] { return 42; }));
    ct::utils::dynamic_assert(ct::throws<std::exception>([] { throw std::runtime_error{"violation"}; }));
    ct::utils::dynamic_assert(not ct::throws<std::exception>([] { throw 42; }));
    ct::utils::dynamic_assert(not ct::throws<std::exception>([] { return 42; }));
}
