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

int main()
{
    auto const t0 = ConversionTracker{true};
    auto const t1 = ConversionTracker{false};
    auto const c0 = ct::lift(t0);
    auto const c1 = ct::lift(t1);

    auto const conjunction = (c1 and c0);
    ct::utils::dynamic_assert(not conjunction);
    ct::utils::dynamic_assert(t1.was_converted());
    ct::utils::dynamic_assert(not t0.was_converted()); // due to short circuiting
    assert_output("( 0 and <unknown> )", conjunction);

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
