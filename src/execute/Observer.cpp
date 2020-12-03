// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "Observer.h"

#include "CaseReporter.h"

namespace clean_test::execute {

void Observer::operator()(Observation observation)
{
    auto const guard = std::lock_guard{m_mutex};
    m_observations.emplace_back(std::move(observation));
    m_reporter(m_observations.back());
}

}
