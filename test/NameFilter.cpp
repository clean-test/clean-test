// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <expression/Throws.h>

#include <execute/Conductor.h>
#include <execute/NameFilter.h>
#include <execute/ColoringSetup.h>

#include <framework.h>

namespace {

using clean_test::framework::Name;
using clean_test::framework::Tag;
using Filter = clean_test::execute::NameFilter;
using Toggle = clean_test::execute::NameFilterToggle;
using Property = clean_test::execute::NameFilterProperty;


auto name(std::string_view path, std::convertible_to<std::string_view> auto &&... tags)
{
    auto result = Name{path};
    return (result /= ... /= Tag{tags});
}

static auto const don = name("knuth/donald");
static auto const rob = name("knuth/rob");
static auto const ada = name("ada", "female", "non-robot", "born-1815");

void empty()
{
    auto const f = Filter{};
    clean_test::utils::dynamic_assert(f(don));
    clean_test::utils::dynamic_assert(f(rob));
    clean_test::utils::dynamic_assert(f(ada));
}

void no_match()
{
    auto const f = [] {
        auto r = Filter{};
        r.add(Toggle::enabled, Property::path, "knuth");
        return r;
    }();

    clean_test::utils::dynamic_assert(f(don));
    clean_test::utils::dynamic_assert(not static_cast<bool>(f(ada)));
}

void conflict()
{
    auto const f = [] {
        auto r = Filter{};
        r.add(Toggle::disabled, Property::path, "rob");
        r.add(Toggle::enabled, Property::path, "knuth");
        return r;
    }();

    clean_test::utils::dynamic_assert(f(don));
    clean_test::utils::dynamic_assert(not static_cast<bool>(f(rob)));
}

void properties()
{
    auto const t = [] {
        auto r = Filter{};
        r.add(Toggle::enabled, Property::tag, "rob");
        return r;
    }();

    auto const a = [] {
        auto r = Filter{};
        r.add(Toggle::enabled, Property::any, "rob");
        return r;
    }();

    clean_test::utils::dynamic_assert(not static_cast<bool>(t(rob)));
    clean_test::utils::dynamic_assert(t(ada));

    clean_test::utils::dynamic_assert(a(rob));
    clean_test::utils::dynamic_assert(a(ada));
}

void pattern()
{
    auto const f = [](std::convertible_to<std::string_view> auto && pattern) {
        auto r = Filter{};
        r.add(Toggle::enabled, Property::any, pattern);
        return r;
    };

    clean_test::utils::dynamic_assert(f("(ada)?ada")(ada)); // pattern with submatch
    clean_test::utils::dynamic_assert(f("ad(?=.*a)")(ada)); // positive look-ahead
    clean_test::utils::dynamic_assert(not static_cast<bool>(f("ADA")(ada))); // always case sensitive
    clean_test::utils::dynamic_assert(f("born-\\d+")(ada)); // numbers
    clean_test::utils::dynamic_assert(clean_test::expression::throws([&] { f("(?P<no>.*)"); })); // invalid pattern
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void execute()
{
    auto const filter = [] {
        auto f = Filter{};
        f.add(Toggle::enabled, Property::path, "^a");
        return f;
    }();
    auto const setup = clean_test::execute::Conductor::Setup{
        .m_colors = clean_test::execute::coloring_setup(clean_test::execute::ColoringMode::automatic),
        .m_num_workers = 0u,
        .m_buffering = clean_test::execute::BufferingMode::off,
        .m_filter = filter,
    };

    clean_test::Test{"a", [] { clean_test::utils::dynamic_assert(true); }};
    clean_test::Test{"b", [] { clean_test::utils::dynamic_assert(false); }};

    clean_test::execute::Conductor{setup}.run();
}

}

int main()
{
    empty();
    no_match();
    conflict();
    properties();
    pattern();
    execute();
}
