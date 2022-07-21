// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <execute/ColoringMode.h>
#include <execute/ColoringSetup.h>
#include <execute/Conductor.h>
#include <execute/NameFilter.h>

#include <clean-test/framework.h>

namespace ct = clean_test;
using namespace ct::literals;

namespace clean_test::execute {
namespace {

constexpr bool contains(std::string_view const haystack, std::string_view needle)
{
    return (haystack.find(needle) != std::string_view::npos);
}

/// Execute registered tests and return captured output of test execution.
std::string report()
{
    auto buffer = std::ostringstream{};
    auto const filter = NameFilter{};
    Conductor{{.m_logger = buffer,
               .m_colors = coloring_setup(ColoringMode::disabled),
               .m_num_workers = 0u,
               .m_buffering = BufferingMode::testcase,
               .m_filter = filter}}
        .run();
    return std::move(buffer).str();
}

void standard()
{
    "success"_test = [] { ct::expect(true); };
    "failure"_test = [] { ct::expect(false); };
    const auto console = report();
    utils::dynamic_assert(contains(console, "2 test-cases"));
    utils::dynamic_assert(contains(console, "[ RUN   ] success"));
    utils::dynamic_assert(contains(console, "[ PASS  ] success"));
    utils::dynamic_assert(contains(console, "[ RUN   ] failure"));
    utils::dynamic_assert(contains(console, "[ FAIL  ] failure"));
    utils::dynamic_assert(contains(console, "other 1 test-case"));
}

void late_registration()
{
    "bob"_test = [] { auto foo = ct::Test{"joe", [] {}}; };
    auto const console = report();
    utils::dynamic_assert(contains(console, "registered late"));
    utils::dynamic_assert(contains(console, "joe"));
    report(); // to clear now registered test-case joe.
}

}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    ct::execute::standard();
    ct::execute::late_registration();
}
