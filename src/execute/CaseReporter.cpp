// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "CaseReporter.h"

#include "Badges.h"
#include "ColorTable.h"

#include <utils/OSyncStream.h>
#include <utils/WithAdaptiveUnit.h>

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
        case CaseStatus::skip:
        default:
            std::terminate();
    }
}

Color color(CaseStatus status)
{
    switch (status) {
        case CaseStatus::pass:
        case CaseStatus::skip:
            return Color::good;
        case CaseStatus::abort:
        case CaseStatus::fail:
            return Color::bad;
        default:
            std::terminate();
    }
}

constexpr bool use_buffering(BufferingMode const mode)
{
    switch (mode) {
        case BufferingMode::off:
            return false;
        case BufferingMode::testcase:
            return true;
        default:
            std::terminate();
    }
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

auto CaseReporter::output() const
{
    return utils::OSyncStream{m_sink};
}

CaseReporter::CaseReporter(Setup const setup) :
    m_setup{setup},
    m_buffer{},
    m_sink{use_buffering(m_setup.m_buffering) ? m_buffer : m_setup.m_output},
    m_is_observation_state_enabled{false, true, true}
{}

CaseReporter::~CaseReporter()
{
    auto buffered = std::move(m_buffer).str();
    if (not buffered.empty()) {
        utils::OSyncStream{m_setup.m_output} << std::move(buffered);
    }
}

void CaseReporter::operator()(Start const & start)
{
    output() << m_setup.m_colors.colored(Color::good, badge(BadgeType::start)) << ' ' << start.m_name << std::endl;
}

void CaseReporter::operator()(Observation const & o)
{
    if (not m_is_observation_state_enabled[o.m_status]) {
        return;
    }

    output() << observation_description[o.m_status] << " in " << o.m_where.file_name() << ':' << o.m_where.line()
             << '\n'
             << o.m_expression_details << (o.m_description.empty() ? "" : "\n") << o.m_description << std::endl;
}

void CaseReporter::operator()(Stop const & stop)
{
    output()
        << m_setup.m_colors.colored(color(stop.m_status), badge(badge_type(stop.m_status))) << ' ' << stop.m_name
        << " (" << std::setprecision(0) << std::fixed << utils::WithAdaptiveUnit{stop.m_wall_time} << ')' << std::endl;
}

}
