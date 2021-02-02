// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include <execute/Configuration.h>
#include <expression.h>

#include "TestUtilities.h"

#include <iostream>

namespace {

constexpr bool contains(std::string_view const subject, std::string_view const substr)
{
    return subject.find(substr) != std::string_view::npos;
}

namespace ct = clean_test;
using Configuration = ct::execute::Configuration;

auto make_filter(std::initializer_list<ct::execute::NameFilterSetting> init)
{
    return std::vector<ct::execute::NameFilterSetting>{init};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Configuration parse(std::string_view const input) {
    auto const args = [](std::string_view data) {
        auto result = std::vector<std::string>{"a.out"};
        if (data.empty()) {
            return result;
        }

        for (auto end = data.find(' '); end != std::string_view::npos; end = data.find(' ')) {
            result.emplace_back(data.cbegin(), data.cbegin() + end);
            data.remove_prefix(end + 1);
        }
        result.emplace_back(data);
        return result;
    }(input);

    std::cout << "Parsing args: [";
    auto sep = std::string_view{};
    for (auto const & a: args) {
        std::cout << sep << '"' << a << '"';
        sep = ", ";
    }
    std::cout << "]\n";

    auto const argv = [&] {
        auto result = std::vector<char const *>{};
        for (auto const & a : args) {
            result.emplace_back(a.data());
        }
        return result;
    }();
    return Configuration::parse(argv.size(), argv.data());
}

void assert_invalid(std::string_view const input, std::string_view const expectation)
{
    bool threw_correctly = false;
    try {
        parse(input);
    } catch (std::invalid_argument const & xcp) {
        ct::utils::dynamic_assert(contains(xcp.what(), expectation));
        threw_correctly = true;
    } catch (...) {
    }
    ct::utils::dynamic_assert(threw_correctly);
}

void assert_valid(auto && getter, std::string_view const input, auto const & expectation)
{
    auto cfg = Configuration{};
    ct::utils::dynamic_assert(not ct::throws([&] { cfg = parse(input); }));
    ct::utils::dynamic_assert(std::forward<decltype(getter)>(getter)(cfg) == expectation);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void operation()
{
    using O = ct::execute::OperationMode;
    auto const get = [](Configuration const & cfg) { return cfg.m_operation; };
    assert_valid(get, "", Configuration{}.m_operation);
    assert_valid(get, "-h", O::help);
    assert_valid(get, "--help", O::help);
    assert_valid(get, "-l", O::list);
    assert_valid(get, "--list", O::list);
    assert_valid(get, "-l --list", O::list);
    assert_valid(get, "--help -h", O::help);

    assert_invalid("-l=true", "Invalid argument");
    assert_invalid("-h=true", "Invalid argument");
    assert_invalid("--list=1", "Invalid argument");
    assert_invalid("--help=0", "Invalid argument");
    assert_invalid("--list yes", "Invalid argument");
    assert_invalid("--help no", "Invalid argument");
    assert_invalid("--help -l", "Contradicting arguments");
    assert_invalid("-h --list", "Contradicting arguments");
}

void color() {
    using C = ct::execute::ColoringMode;
    auto const get = [](Configuration const & cfg) { return cfg.m_coloring; };
    assert_valid(get, "", Configuration{}.m_coloring);
    assert_valid(get, "--color auto", C::automatic);
    assert_valid(get, "--color=automatic", C::automatic);
    assert_valid(get, "--color never", C::disabled);
    assert_valid(get, "--color=always", C::enabled);

    assert_invalid("--color no", "Invalid argument");
    assert_invalid("--color=0", "Invalid argument");
    assert_invalid("--color ", "Invalid argument");
    assert_invalid("--color=", "Missing mandatory details");
    assert_invalid("--color", "Missing mandatory details");
    assert_invalid("--color=auto --color never", "Contradicting arguments");
    assert_invalid("--color always --color=automatic", "Contradicting arguments");
}

void filter()
{
    using T = ct::execute::NameFilterToggle;
    using P = ct::execute::NameFilterProperty;
    auto const get = [](Configuration const & cfg) -> decltype(auto) { return cfg.m_filter_settings; };
    assert_valid(get, "", Configuration{}.m_filter_settings);
    assert_valid(get, "--filter A", make_filter({{T::enabled, P::any, "A"}}));
    assert_valid(get, "--filter +path:B", make_filter({{T::enabled, P::path, "B"}}));
    assert_valid(get, "--filter -tag:C", make_filter({{T::disabled, P::tag, "C"}}));
    assert_valid(get, "--filter -any:D", make_filter({{T::disabled, P::any, "D"}}));
    assert_valid(get, "--filter +E --filter +E", make_filter({{T::enabled, P::any, "E"}, {T::enabled, P::any, "E"}}));
    assert_valid(
        get,
        "--filter +path:F --filter -tag:F/F",
        make_filter({{T::enabled, P::path, "F"}, {T::disabled, P::tag, "F/F"}}));

    assert_invalid("--filter", "Missing mandatory details");
    assert_invalid("--filter=", "Missing mandatory details");
    assert_invalid("--filter ", "Invalid argument");
    assert_invalid("--filter +path:", "Invalid argument");
}

void buffering()
{
    static constexpr auto enabled = ct::execute::BufferingMode::testcase;
    auto const get = [](Configuration const & cfg) { return cfg.m_buffering; };
    assert_valid(get, "", Configuration{}.m_buffering);
    assert_valid(get, "-b", enabled);
    assert_valid(get, "--buffered", enabled);
    assert_valid(get, "-b --buffered", enabled);

    assert_invalid("-b=true", "Invalid argument");
    assert_invalid("--buffered=1", "Invalid argument");
    assert_invalid("--buffered yes", "Invalid argument");
    assert_invalid("--buffered ", "Invalid argument");
}

void threads()
{
    auto const get = [](Configuration const & cfg) { return cfg.m_num_jobs; };
    assert_valid(get, "", Configuration{}.m_num_jobs);
    assert_valid(get, "--jobs=0", 0ul);
    assert_valid(get, "--jobs 2", 2ul);
    assert_valid(get, "--jobs 5 --jobs=5", 5ul);
    assert_valid(get, "-j 4", 4ul);

    assert_invalid("-j=4", "Invalid argument");
    assert_invalid("--jobs4", "Invalid argument");
    assert_invalid("--jobs=4ul", "Invalid argument");
    assert_invalid("--jobs=-18", "Invalid argument");
    assert_invalid("--jobs ", "Invalid argument");
    assert_invalid("--jobs=4 --jobs=5", "Contradicting arguments");
    assert_invalid("--jobs=", "Missing mandatory details");
    assert_invalid("--jobs", "Missing mandatory details");
    assert_invalid("--jobs --color", "Invalid argument");
}

void report()
{
    auto const get = [](Configuration const & cfg) { return cfg.m_junit_path; };
    assert_valid(get, "", "");
    assert_valid(get, "--report a.xml", "a.xml");
    assert_valid(get, "--report=b.xml", "b.xml");
    assert_valid(get, "--report=c.xml --report=c.xml", "c.xml");
    assert_valid(get, "--report=junit:d.xml", "d.xml");
    assert_valid(get, "--report junit:e.xml", "e.xml");
    assert_valid(get, "--report junit:f.xml --report=f.xml", "f.xml");

    assert_invalid("--report=", "Missing mandatory details");
    assert_invalid("--report", "Missing mandatory details");
    assert_invalid("--report g.xml --report=junit:h.xml", "Contradicting arguments");
    assert_invalid("--report ", "Invalid argument");
}

void depth()
{
    auto const get = [](Configuration const & cfg) { return cfg.m_depth; };
    assert_valid(get, "", Configuration{}.m_depth);
    assert_valid(get, "--depth=0", 0ul);
    assert_valid(get, "--depth 2", 2ul);
    assert_valid(get, "--depth 5 --depth=5", 5ul);
    assert_valid(get, "-d 4", 4ul);

    assert_invalid("-d=4", "Invalid argument");
    assert_invalid("--depth4", "Invalid argument");
    assert_invalid("--depth=4ul", "Invalid argument");
    assert_invalid("--depth=-18", "Invalid argument");
    assert_invalid("--depth ", "Invalid argument");
    assert_invalid("--depth=4 --depth=5", "Contradicting arguments");
    assert_invalid("--depth=", "Missing mandatory details");
    assert_invalid("--depth", "Missing mandatory details");
    assert_invalid("--depth --color", "Invalid argument");
}

void combined_short_knobs()
{
    auto const get = [](Configuration const & cfg) { return std::tuple{cfg.m_operation, cfg.m_depth}; };

    assert_valid(get, "-ld 4", std::tuple{ct::execute::OperationMode::list, 4u});
    assert_valid(get, "-hd 0", std::tuple{ct::execute::OperationMode::help, 0u});

    assert_invalid("-hld 4", "Contradicting arguments");
    assert_invalid("-foo 4", "Invalid argument");
    assert_invalid("-dh 4", "Invalid argument");
    assert_invalid("-dl 4", "Invalid argument");
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    operation();
    color();
    filter();
    buffering();
    threads();
    report();
    depth();

    combined_short_knobs();
}
