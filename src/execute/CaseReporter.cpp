// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "CaseReporter.h"

#include "Badges.h"

#include "utils/WithAdaptiveUnit.h"

#include <iomanip>
#include <ostream>

namespace clean_test::execute {
namespace {

constexpr auto observation_description = std::array{
    "Passing expectation",
    "Failure (flaky)",
    "Failure",
};

BadgeType badge_type(CaseStatus status)
{
    switch (status) {
        case CaseStatus::abort:
            return BadgeType::abort;
        case CaseStatus::fail:
            return BadgeType::fail;
        case CaseStatus::pass:
            return BadgeType::pass;
        default:
            std::terminate();
    }
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CaseReporter::CaseReporter(std::ostream & output) : m_output{output}, m_is_observation_state_enabled{false, true, true}
{}

void CaseReporter::operator()(Start const & start)
{
    m_output << badge(BadgeType::start) << ' ' << start.m_name << std::endl;
}

void CaseReporter::operator()(Observation const & o)
{
    if (not m_is_observation_state_enabled[o.m_status]) {
        return;
    }

    m_output << observation_description[o.m_status] << " in " << o.m_where.file_name() << ':' << o.m_where.line()
             << '\n'
             << o.m_expression_details << (o.m_description.empty() ? "" : "\n") << o.m_description << std::endl;
}

void CaseReporter::operator()(Stop const & stop)
{
    m_output << badge(badge_type(stop.m_status)) << ' ' << stop.m_name << " (" << std::setprecision(0) << std::fixed
             << utils::WithAdaptiveUnit{stop.m_wall_time} << ')' << std::endl;
}

}
