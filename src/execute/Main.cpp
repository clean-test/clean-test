// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "execute/Main.h"

#include <execute/ColoringSetup.h>
#include <execute/Conductor.h>
#include <execute/Configuration.h>
#include <execute/NameFilter.h>

#include <algorithm>
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

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
    return main(Configuration::parse(argc, argv));
}

int main(Configuration const & configuration)
{
    auto const & colors = coloring_setup(configuration.m_coloring);
    auto const filter = NameFilter{configuration.m_filter_settings};
    auto const conductor = Conductor{{
        .m_colors = colors,
        .m_num_workers = configuration.m_num_jobs,
        .m_buffering = configuration.m_buffering,
        .m_filter = filter}};
    auto results = conductor.run();
    return count_if(results, [](auto const & r) { return r.m_status != CaseStatus::pass; });
}

}
