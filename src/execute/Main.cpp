// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "execute/Main.h"

#include <framework/Registry.h>

#include <execute/Badges.h>
#include <execute/ColoringSetup.h>
#include <execute/Conductor.h>
#include <execute/Configuration.h>
#include <execute/HelpDisplay.h>
#include <execute/JUnitExport.h>
#include <execute/NameFilter.h>
#include <execute/TreeDisplay.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <version>

namespace clean_test::execute {
namespace {

/// Variant of @c std::count_if for whole containers.
template <typename Data, std::invocable<typename Data::value_type> Predicate>
constexpr std::size_t count_if(Data const & data, Predicate && predicate)
{
#if __cpp_lib_ranges
    return std::ranges::count_if(data, std::forward<Predicate>(predicate));
#else
    return std::count_if(std::cbegin(data), std::cend(data), std::forward<Predicate>(predicate));
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ColorTable const & load_colors(Configuration const & cfg)
{
    return coloring_setup(cfg.m_coloring);
}

auto filter(Configuration const & cfg)
{
    return NameFilter{cfg.m_filter_settings};
}

void serialize(std::filesystem::path const & path, ColorTable const & colors, auto && data) {
    if (path.empty()) {
        return;
    }

    auto out = std::ofstream{path, std::ios_base::trunc | std::ios_base::out};
    if (not out) {
        std::cout
            << colors[Color::bad] << badge(BadgeType::headline) << colors[Color::off]
            << " Error: Failed to write report into " << path << ".\n";
        return;
    }
    out << data;
}

int run(Configuration const & cfg)
{
    auto const & colors = load_colors(cfg);
    auto const conductor = Conductor{{
        .m_colors = colors,
        .m_num_workers = cfg.m_num_jobs,
        .m_buffering = cfg.m_buffering,
        .m_filter = filter(cfg)}};

    auto results = conductor.run();
    serialize(cfg.m_junit_path, colors, JUnitExport{{Outcome::Duration{}, results}});

    return count_if(results, [](auto const & r) { return r.m_status != CaseStatus::pass; });
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
    try {
        return main(Configuration::parse(argc, argv));
    } catch (std::invalid_argument const & xcp) {
        std::cout
            << coloring_setup(ColoringMode::automatic).colored(Color::bad, "ERROR")
            << ": Failed to parse command line. " << xcp.what() << '\n';
        return 1;
    }
}

int main(Configuration const & cfg)
{
    switch (cfg.m_operation) {
        case OperationMode::help:
            std::cout << HelpDisplay{load_colors(cfg)};
            break;

        case OperationMode::list:
            std::cout << TreeDisplay{framework::registry(), {load_colors(cfg), filter(cfg), cfg.m_depth}};
            break;

        case OperationMode::run:
            return run(cfg);

        default:
            std::terminate();
    }
    return 0;
}

}
