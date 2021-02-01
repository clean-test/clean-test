// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "Observer.h"

#include "CaseReporter.h"

namespace clean_test::execute {

void Observer::operator()(Observation observation)
{
    m_reporter(observation);
    auto const guard = m_observations.guard();
    guard->emplace_back(std::move(observation));
}

}
