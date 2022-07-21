// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "CaseEvaluator.h"

#include "Abortion.h"
#include "CaseReporter.h"
#include "Observer.h"

namespace clean_test::execute {

CaseEvaluator::CaseEvaluator(CaseReporter::Setup const setup) : m_reporter{setup}
{}

CaseResult CaseEvaluator::operator()(framework::Case & tc) noexcept
{
    using Clock = CaseResult::Clock;

    auto observer = Observer{m_reporter};
    m_reporter(CaseReporter::Start{tc.name().path()});
    auto execution_outcome = CaseStatus{CaseStatus::pass};
    auto const time_start = Clock::now();
    try {
        tc.run(observer);
    } catch (execute::Abortion const &) {
        execution_outcome = CaseStatus::abort;
    } catch (std::exception const &) {
        execution_outcome = CaseStatus::abort;
    } catch (...) {
        execution_outcome = CaseStatus::abort;
    }
    auto const wall_time = Clock::now() - time_start;

    auto result = CaseResult{
        std::string{tc.name().path()}, execution_outcome, wall_time, std::move(observer).release()};
    m_reporter(CaseReporter::Stop{tc.name().path(), wall_time, result.m_status});
    return result;
}

}
