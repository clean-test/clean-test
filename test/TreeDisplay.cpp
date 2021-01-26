// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include <execute/TreeDisplay.h>
#include <execute/ColoringSetup.h>

#include "TestUtilities.h"

#include <clean-test/framework.h>

#include <sstream>
#include <iostream>

namespace {

namespace ct = clean_test;
using namespace ct::literals;

auto const t0 = ct::Test("b") = [] {};
auto const t1 = ct::Test("a/b/c") = [] {};
auto const t2 = ct::Test("a/b") = [] {};
auto const t3 = ct::Test("a/a/b") = [] {};
auto const t4 = ct::Test("a/a") = [] {};
auto const t5 = ct::Test("a") = [] {};

enum class Pattern { all, some, none };

std::string_view pattern(Pattern const p)
{
    switch (p) {
        case Pattern::all:
            return ".*";
        case Pattern::some:
            return "^a";
        case Pattern::none:
            return "^c";
        default:
            std::terminate();
    }
}

template <std::invocable<std::string_view> App>
void for_lines(std::string_view input, App && app)
{
    for (auto end = input.find('\n'); end != std::string_view::npos; input.remove_prefix(end + 1ul), end = input.find('\n')) {
        app(input.substr(0ul, end));
    }
    if (not input.empty()) {
        app(input);
    }
}

std::size_t count_lines(std::string_view const input)
{
    auto result = 0ul;
    for_lines(input, [&result](auto &&) { ++result; });
    return result;
}

void test(std::size_t const depth, Pattern const p) {
    auto const tree = [&] {
        auto const & registry = ct::framework::registry();
        auto const & colors = ct::execute::coloring_setup(ct::execute::ColoringMode::disabled);
        auto const filter = [&] {
            auto result = ct::execute::NameFilter{};
            if (p != Pattern::all) {
                result.add(ct::execute::NameFilterToggle::enabled, ct::execute::NameFilterProperty::path, pattern(p));
            }
            return result;
        }();

        auto buffer = std::stringstream{};
        buffer << ct::execute::TreeDisplay{registry, {.m_colors = colors, .m_filter = filter, .m_depth = depth}};
        return std::move(buffer).str();
    }();

    std::cout << "Tree " << depth << ", " << static_cast<std::underlying_type_t<Pattern>>(p) << ":\n" << tree;

    if (depth != 0) {
        for_lines(tree, [&](auto const line) {
            auto const expected_length
                = 2ul // the default indent
                + (depth - 1ul) * (3ul + 3ul) // the tree symbols: 3-byte unicode-magic plus 3-space padding
                + (3ul + 1ul + 1ul); // another tree symbol, a whitespace and the name (always one char in this test)
            ct::utils::dynamic_assert(line.size() <= expected_length);
        });
    }

    auto const expected_num_lines = [&] {
        switch (p) {
            case Pattern::all:
                return 6ul;
            case Pattern::some:
                return 5ul;
            case Pattern::none:
                return 0ul;
            default:
                std::terminate();
        }
    }();
    ct::utils::dynamic_assert(count_lines(tree) <= expected_num_lines);
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    for (auto i = 0ul; i < 5ul; ++i) {
        for (auto const pattern : {Pattern::all, Pattern::some, Pattern::none}) {
            test(i, pattern);
        }
    }
}
