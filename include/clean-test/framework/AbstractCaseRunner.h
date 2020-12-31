// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ObserverFwd.h"

namespace clean_test {

/// Interface for case runners i.e. function objects defining test-cases.
class AbstractCaseRunner {
public:
    virtual ~AbstractCaseRunner() = default;

    /// Invoke runner with given @p observer.
    void run(execute::Observer & observer)
    {
        return run_impl(observer);
    }

private:
    /// implementation helper for the template method design idiom.
    virtual void run_impl(execute::Observer & observer) = 0;
};

}
