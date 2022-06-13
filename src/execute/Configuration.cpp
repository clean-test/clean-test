// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "execute/Configuration.h"

#include <array>
#include <charconv>
#include <optional>
#include <unordered_map>

namespace clean_test::execute {
namespace {

std::string compose(auto &&... value)
{
    auto result = std::string{};
    (result.append(std::forward<decltype(value)>(value)), ...);
    return result;
}

enum class Report {
    JUnit
};

std::filesystem::path & report_sink(Configuration & cfg, Report const report)
{
    switch (report) {
        case Report::JUnit:
            return cfg.m_junit_path;
        default:
            std::terminate();
    }
}

auto load_possible_short_knobs()
{
    auto buffer = std::string{};
    for (auto c = 'a'; c <= 'z'; ++c) {
        buffer.push_back(c);
    }
    for (auto c = 'A'; c <= 'Z'; ++c) {
        buffer.push_back(c);
    }
    return buffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Collection of parsing utilities and common error reporting facilities.
class ParserUtilities {
public:
    using View = std::string_view;

    [[noreturn]] static void error(auto &&... args)
    {
        throw std::invalid_argument{compose(std::forward<decltype(args)>(args)...)};
    }

    static bool is_short_knob(View const candidate)
    {
        static auto const possible_short_knobs = load_possible_short_knobs();
        return candidate.size() > 1ul and candidate[0] == '-'
               and candidate.substr(1).find_first_not_of(possible_short_knobs) == View::npos;
    }

protected:
    [[noreturn]] static void contradiction(View const name, View const candidate, View const reference)
    {
        error("Contradicting arguments for ", name, ": ", candidate, " conflicts with ", reference, ".");
    }

    [[noreturn]] static void contradiction(View const candidate, View const reference)
    {
        error("Contradicting arguments: ", candidate, " conflicts with ", reference, ".");
    }

    [[noreturn]] static void invalid(View const argument)
    {
        error("Invalid argument '", argument, "'.");
    }

    [[noreturn]] static void invalid(View const name, View const argument)
    {
        error("Invalid argument '", argument, "' for ", name, ".");
    }

    /// Determine whether @p candidate enables any of the @p knobs and does not conflict with @p reference.
    static bool enables(View & reference, View const candidate, View const long_knob, char const short_knob)
    {
        auto const result
            = (candidate == long_knob or (is_short_knob(candidate) and candidate.find(short_knob) != View::npos));
        if (result) {
            if (not reference.empty() and (reference != long_knob)) {
                contradiction(candidate, reference);
            }
            reference = long_knob;
        }
        return result;
    }

    /// Determine whether @p candidate starts either the @p short_knob or the @p long_knob.
    ///
    /// The default @p short_knob disables handling the short_knob and only honors the long_knob.
    static std::optional<View> starts_option(View const candidate, View const long_knob, char const short_knob = '\0')
    {
        if (candidate.starts_with(long_knob)) {
            if (candidate.size() == long_knob.size()) {
                return View{};
            }
            if (candidate[long_knob.size()] != '=') {
                invalid(candidate);
            }
            return candidate.substr(long_knob.size() + 1);
        }
        if (short_knob != '\0' and is_short_knob(candidate) and candidate.back() == short_knob) {
            return View{};
        }
        return {};
    }

    /// Convert @p input to @c std::size_t.
    ///
    /// Report error if this contradicts the previous value for @p description stored in @p reference.
    static std::size_t parse_size(View & reference, View const input, View const description)
    {
        if (not reference.empty() and reference != input) {
            contradiction(description, input, reference);
        }

        auto result = 0ul;
        if (auto [end, ec] = std::from_chars(input.cbegin(), input.cend(), result);
            ec != std::errc{} or end != input.cend()) {
            invalid(description, input);
        }
        reference = input;
        return result;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Commandline parser powering @c Configuration::parse.
///
/// Provides three different kinds of helpers for various commandline-knobs (placeholder X here):
///  - enables_X: for arguments without parameters (e.g. --help).
///  - starts_X: for identifying whether parameters for argument X are specified right after (e.g. --filter foo).
///  - X: for importing the parameter for arguments after they have been started (see above).
class Parser : private ParserUtilities {
public:
    bool enables_help(View const candidate)
    {
        return enables(m_operation, candidate, "--help", 'h');
    }

    bool enables_listing(View const candidate)
    {
        return enables(m_operation, candidate, "--list", 'l');
    }

    std::optional<View> starts_coloring(View const candidate)
    {
        return starts_option(candidate, "--color");
    }

    ColoringMode coloring(View const specification)
    {
        using C = ColoringMode;
        static auto const lookup = std::unordered_map<View, C>{
            {"auto", C::automatic}, {"automatic", C::automatic}, {"always", C::enabled}, {"never", C::disabled}};
        if (auto const pos = lookup.find(specification); pos != lookup.cend()) {
            if (not m_coloring.first.empty() and m_coloring.second != pos->second) {
                contradiction("color", m_coloring.first, pos->first);
            }
            m_coloring = *pos;
            return pos->second;
        }
        invalid("color", specification);
    }

    std::optional<View> starts_filter(View const candidate)
    {
        return starts_option(candidate, "--filter", 'f');
    }

    NameFilterSetting filter(View candidate)
    {
        if (candidate.empty()) {
            invalid("filter", candidate);
        }

        auto const toggle = [&] {
            switch (candidate.front()) {
                case '-':
                    candidate.remove_prefix(1);
                    return NameFilterToggle::disabled;
                case '+':
                    candidate.remove_prefix(1);
                    [[fallthrough]];
                default:
                    return NameFilterToggle::enabled;
            }
        }();

        auto const property = [&] {
            static auto constexpr properties = std::to_array<View>({"path:", "tag:", "any:"});
            for (auto p = 0ul; p < properties.size(); ++p) {
                auto const prop = properties[p];
                if (candidate.starts_with(prop)) {
                    candidate.remove_prefix(prop.size());
                    return static_cast<NameFilterProperty>(p);
                }
            }
            return NameFilterProperty::any;
        }();

        if (candidate.empty()) {
            invalid("filter pattern", candidate);
        }
        return {toggle, property, std::string{candidate}};

    }

    bool enables_buffering(View const v)
    {
        return enables(m_buffering, v, "--buffered", 'b');
    }

    std::optional<View> starts_threads(View const candidate)
    {
        return starts_option(candidate, "--jobs", 'j');
    }

    std::size_t threads(View const candidate)
    {
        return parse_size(m_threads, candidate, "jobs");
    }

    std::optional<View> starts_report(View const candidate)
    {
        return starts_option(candidate, "--report");
    }

    std::pair<Report, std::filesystem::path> report(View candidate)
    {
        auto checked = [this](View const result) {
            if (not m_report.empty() and m_report != result) {
                contradiction("report", m_report, result);
            }
            if (result.empty()) {
                invalid("report path", result);
            }
            m_report = result;
            return result;
        };

        using C = std::pair<View, Report>;
        for (auto [prefix, type] : {C{"junit:", Report::JUnit}})
            if (candidate.starts_with(prefix)) {
                return {type, checked(candidate.substr(prefix.size()))};
            }
        return {Report::JUnit, checked(candidate)};
    }

    std::optional<View> starts_depth(View const candidate)
    {
        return starts_option(candidate, "--depth", 'd');
    }

    std::size_t depth(View const candidate)
    {
        return parse_size(m_depth, candidate, "depth");
    }

    [[noreturn]] void unknown_argument(View const v)
    {
        invalid(v);
    }

private:
    View m_operation;
    std::pair<View, ColoringMode> m_coloring;
    View m_buffering;
    View m_threads;
    View m_report;
    View m_depth;
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Configuration Configuration::parse(int const argc, char const * const * const argv)
{
    auto result = Configuration{};
    auto parser = Parser{};
    for (auto cur = 1; cur < argc; ++cur) {
        auto option = [&cur, argc, argv](std::string_view const opt, std::string_view const error) -> std::string_view {
            if (not opt.empty()) {
                return opt;
            }
            if (cur + 1 < argc) {
                return argv[++cur];
            }
            ParserUtilities::error("Missing mandatory details for ", error, ".");
        };

        auto v = std::string_view{argv[cur]};
        auto numHandlers = 0ul;
        if (parser.enables_help(v)) {
            result.m_operation = OperationMode::help;
            ++numHandlers;
        }
        if (parser.enables_listing(v)) {
            result.m_operation = OperationMode::list;
            ++numHandlers;
        }
        if (auto coloring = parser.starts_coloring(v); coloring) {
            result.m_coloring = parser.coloring(option(*coloring, "color"));
            ++numHandlers;
        }
        if (auto filter = parser.starts_filter(v); filter) {
            result.m_filter_settings.emplace_back(parser.filter(option(*filter, "filter")));
            ++numHandlers;
        }
        if (parser.enables_buffering(v)) {
            result.m_buffering = BufferingMode::testcase;
            ++numHandlers;
        }
        if (auto threads = parser.starts_threads(v); threads) {
            result.m_num_jobs = parser.threads(option(*threads, "threads"));
            ++numHandlers;
        }
        if (auto report = parser.starts_report(v); report) {
            auto const [type, destination] = parser.report(option(*report, "report"));
            report_sink(result, type) = destination;
            ++numHandlers;
        }
        if (auto depth = parser.starts_depth(v); depth) {
            result.m_depth = parser.depth(option(*depth, "depth"));
            ++numHandlers;
        }
        if (numHandlers != (ParserUtilities::is_short_knob(v) ? v.size() - 1u : 1u)) {
            parser.unknown_argument(v);
        }
    }
    return result;
}

}
