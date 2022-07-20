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

#include <utils/RangesUtils.h>

#include <iostream>
#include <fstream>

namespace clean_test::execute {
namespace {

ColorTable const & load_colors(Configuration const & cfg)
{
    return coloring_setup(cfg.m_coloring);
}

auto filter(Configuration const & cfg)
{
    return NameFilter{cfg.m_filter_settings};
}

void serialize(std::ostream & logger, std::filesystem::path const & path, ColorTable const & colors, auto && data) {
    if (path.empty()) {
        return;
    }

    auto out = std::ofstream{path, std::ios_base::trunc | std::ios_base::out};
    if (not out) {
        logger
            << colors[Color::bad] << badge(BadgeType::headline) << colors[Color::off]
            << " Error: Failed to write report into " << path << ".\n";
        return;
    }
    out << data;
}

int run(std::ostream & logger, Configuration const & cfg)
{
    auto const & colors = load_colors(cfg);
    auto const conductor = Conductor{{
        .m_logger = logger,
        .m_colors = colors,
        .m_num_workers = cfg.m_num_jobs,
        .m_buffering = cfg.m_buffering,
        .m_filter = filter(cfg)}};

    auto outcome = conductor.run();
    serialize(logger, cfg.m_junit_path, colors, JUnitExport{outcome});

    return static_cast<int>(std::min<std::size_t>(
        std::numeric_limits<int>::max(),
        utils::count_if(outcome.m_results, [](auto const & r) { return r.m_status != CaseStatus::pass; })));
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
    auto & logger = (cfg.m_logger ? *cfg.m_logger : std::cout);
    switch (cfg.m_operation) {
        case OperationMode::help:
            logger << HelpDisplay{load_colors(cfg)};
            break;

        case OperationMode::list:
            logger << TreeDisplay{framework::registry(), {load_colors(cfg), filter(cfg), cfg.m_depth}};
            break;

        case OperationMode::run:
            return run(logger, cfg);

        default:
            std::terminate();
    }
    return 0;
}

}
