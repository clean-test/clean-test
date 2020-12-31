// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ConcreteCaseRunner.h"
#include "Name.h"
#include "ObserverFwd.h"

#include <memory>

namespace clean_test::framework {

/// Internal wrapper comprising all relevant details for a test-case
///
/// This includes static details (e.g. the actual underlying test-runner) and dynamic results computed at runtime.
class Case {
public:
    using Runner = std::unique_ptr<AbstractCaseRunner>;

    Case(Name name, Runner runner) noexcept : m_name{std::move(name)}, m_runner{std::move(runner)}
    {}

    Name const & name() const
    {
        return m_name;
    }

    void run(execute::Observer& observer)
    {
        m_runner->run(observer);
        // make sure the runner is only ever used once.
        m_runner.reset();
    }

private:
    Name m_name;
    Runner m_runner;
};

}
