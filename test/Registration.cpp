// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <clean-test/framework.h>

#include <iostream>
#include <set>
#include <sstream>

namespace {

namespace ct = clean_test;
using namespace ct::literals;

// It doesn't get simpler than this: Start with our first test-suite
auto const short_suite = ct::Suite{"short_suite", [] {
    // ... and register test-case "short_suite/short".
    "short"_test = [] {};
}};

// Alternative syntax of registering test-cases
auto const test_alternatives = ct::Suite{"test_alternatives", [] {
    "foo"_test = [] {};
    ct::Test{"bar"} = [] {};
    ct::Test{"baz", [] {}};
}};

// The same works for test-suites (although less conveniently on namespace-scope)
auto const suite_alternative_0 = ct::Suite{"suite_alternative_0", [] {
    "foo"_test = [] {};
}};
auto const suite_alternative_1 = ct::Suite{"suite_alternative_1"} = [] {
    "foo"_test = [] {};
};
auto const suite_alternative_2 = "suite_alternative_2"_suite = [] {
    "foo"_test = [] {};
};

// Test-cases are not required to be nested within some suite.
auto const free = ct::Test("free") = [] {};

// Suites may be nested: this registers "parent/child/foo".
auto const suite_parent = ct::Suite{"parent", [] {
    auto const suite_child = ct::Suite{"child", [] {
        "foo"_test = [] {};
    }};
}};

// Suites and cases can be registered from function calls.
auto const ignore_0 = [] {
    "IELE"_suite = [] {
        "addendum"_test = [] {};
    };
    return 0;
}();

// ... but the user has to make sure the function actually gets called.
// (Below example won't get registered, since the lambda isn't invoked.)
[[maybe_unused]] auto const ignore_1 = [] {
    "not_registered"_suite = [] {
        "invisible"_test = [] { std::cout << "invisible" << std::endl; };
    };
};

// Both cases and suites support tags (both via Tag and UDL).
auto const tagged = ct::Suite{ct::Tag{"tag"} / "tagged", [] {
    "taxy"_tag / "foo"_test = [] {};
}};

// Tags as well as names can be nested i.e. joined
auto const suite_tagged_0 = ct::Suite{ct::Tag{"tag0"} / "tagged", [] {
    "tag1"_tag / "foo"_test = [] {};
    ct::Tag{"tag1"} / "nested" / "foo"_test = [] {};
    "tag1"_tag / "nested"_tag / "bar"_test = [] {};
}};
auto const suite_tagged_1 = "tag2"_tag / "tagged"_suite = [] { "foo"_test = [] {}; };
auto const suite_tagged_2 = "tag2"_tag / "nested" / "tagged"_suite = [] { "foo"_test = [] {}; };
auto const suite_tagged_3 = "tag2"_tag / "nested"_tag / "tagged"_suite = [] { "foo"_test = [] {}; };

// Registration doesn't ensure name or tag uniqueness.
auto const duplicate_0 = ct::Suite{"duplicate", [] {"case"_test = [] {}; }};
auto const duplicate_1 = ct::Suite{"duplicate", [] {
    auto const test = "case"_test;
    test = [] {};
    test = [] {};
}};

// Empty suites just work fine (and just do nothing -- surprise!)
auto const suite_empty = ct::Suite{"empty", [] {}};

// with explicit observer
auto const observability = ct::Suite{"observer", [] {
    "test"_test = [](ct::Observer &) {};
}};

} // anonymous namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string description(ct::framework::Name const & name) {
    auto buffer = std::ostringstream{};
    buffer << name.path();
    if (auto const & tags = name.tags(); not tags.empty()) {
        auto sep = std::string_view{" {"};
        for (auto const & tag: tags) {
            buffer << sep << std::string_view{tag};
            sep = ", ";
        }
        buffer << "}";
    }
    return buffer.str();
}

template <typename T>
auto set_difference(std::vector<T> const & lhs, std::vector<T> const & rhs)
{
    auto result = std::vector<T>{};
    std::set_difference(std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs), std::back_inserter(result));
    return result;
}

template <typename T>
constexpr auto sorted(std::vector<T> data)
{
    std::sort(data.begin(), data.end());
    return data;
}

// Ensure invariants of registered test-suites and -cases as documented above.
int main()
{
    // per TU: ordered from top to bottom.
    auto const expected_descriptions = sorted(std::vector<std::string>{
        // this TU
        "short_suite/short",
        "test_alternatives/foo",
        "test_alternatives/bar",
        "test_alternatives/baz",
        "suite_alternative_0/foo",
        "suite_alternative_1/foo",
        "suite_alternative_2/foo",
        "free",
        "parent/child/foo",
        "IELE/addendum",
        "tagged/foo {tag, taxy}",
        "tagged/foo {tag0, tag1}",
        "tagged/nested/foo {tag0, tag1}",
        "tagged/bar {tag0, tag1, nested}",
        "tagged/foo {tag2}",
        "nested/tagged/foo {tag2}",
        "tagged/foo {tag2, nested}",
        "duplicate/case",
        "duplicate/case",
        "duplicate/case",
        "observer/test",

        // addendum TU
        "second_tu/case",
    });

    auto found_descriptions = sorted([&] {
        auto result = std::vector<std::string>{};
        auto const & registry = ct::framework::registry();
        for (auto && test_case : registry) {
            result.emplace_back(description(test_case.name()));
        }
        return result;
    }());

    auto const missing_descriptions = set_difference(expected_descriptions, found_descriptions);
    auto const excess_description = set_difference(found_descriptions, expected_descriptions);
    if (not missing_descriptions.empty()) {
        std::cout << "Missing (expected but not found):\n";
        for (auto const & name: missing_descriptions) {
            std::cout << "  " << name << "\n";
        }
    }
    if (not excess_description.empty()) {
        std::cout << "Excess (found but not expected):\n";
        for (auto const & name: excess_description) {
            std::cout << "  " << name << "\n";
        }
    }
    ct::utils::dynamic_assert(found_descriptions == expected_descriptions);
}
