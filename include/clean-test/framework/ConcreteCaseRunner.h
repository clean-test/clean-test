// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "AbstractCaseRunner.h"
#include "ObserverFwd.h"

#include <concepts>
#include <utility>

namespace clean_test::framework {

/// Implementation of the @c AbstractCaseRunner interface based on @tparam F.
template <std::invocable<execute::Observer &> F>
class ConcreteCaseRunner final : public AbstractCaseRunner {
public:
    virtual ~ConcreteCaseRunner() = default;

    /// Combined move and copy c'tor from @tparam F @p func.
    explicit ConcreteCaseRunner(std::convertible_to<F> auto && func) : m_func{std::forward<decltype(func)>(func)}
    {}

private:
    /// Invoke by passing @p observer to the stored @tparam F @c m_func.
    void run_impl(execute::Observer & observer) final
    {
        m_func(observer);
    }

    F m_func;
};

}
