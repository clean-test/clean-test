// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <execute/Conductor.h>

#include <clean-test/clean-test.h>

#include <map>

namespace {

namespace ct = clean_test;

auto const name_prefix = std::string{"auto-test-"};

auto auto_test()
{
    static auto num = 0ul;
    return ct::Test{name_prefix + std::to_string(num++)};
}

auto const dummy = [] {
    auto_test() = [] { ct::expect(true); };
    auto_test() = [] { ct::expect(false); };
    auto_test() = [] { ct::expect(true) << ct::asserted; ct::expect(true); };
    auto_test() = [] { ct::expect(false) << ct::asserted; ct::expect(true); };
    auto_test() = [] { ct::expect(true) << "Can't see this"; };
    auto_test() = [] { ct::expect(false) << "Can see this!"; };
    auto_test() = [] { ct::expect(true) << ct::flaky; };
    auto_test() = [] { ct::expect(false) << ct::flaky; };
    auto_test() = [] { ct::expect(false) << ct::flaky << ct::asserted; };

    // Recursion
    auto_test() = [] { ct::expect(ct::throws([] { ct::expect(ct::throws([] { throw 13; })); })); };

    // Explicit observer
    auto_test() = [](ct::Observer & o) { ct::expect(o, true); };
    auto_test() = [](ct::Observer & o) { ct::expect(o, false); };

    return 1;
}();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

auto load_positions(std::vector<ct::execute::CaseResult> const & results)
{
    auto positions = std::map<std::string, std::size_t>{};
    for (auto i = 0ul; i < results.size(); ++i) {
        positions[results[i].m_name_path] = i;
    }
    return positions;
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    using Outcome = ct::execute::CaseStatus;
    using State = ct::execute::ObservationStatus;

    auto const results = ct::execute::Conductor{}.run();
    auto test = [&results, pos = load_positions(results), cur = 0ul](auto && asserter) mutable {
        asserter(results.at(pos[name_prefix + std::to_string(cur++)]));
    };

    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::pass);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::pass);
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::fail);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::fail);
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::pass);
        ct::utils::dynamic_assert(result.m_observations.size() == 2ul);
        for (auto const & observation : result.m_observations) {
            ct::utils::dynamic_assert(observation.m_status == State::pass);
        }
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::abort);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::fail_asserted);
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::pass);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        auto const & observation = result.m_observations[0];
        ct::utils::dynamic_assert(observation.m_status == State::pass);
        ct::utils::dynamic_assert(observation.m_description == "Can't see this");
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::fail);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        auto const & observation = result.m_observations[0];
        ct::utils::dynamic_assert(observation.m_status == State::fail);
        ct::utils::dynamic_assert(observation.m_description == "Can see this!");
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::pass);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::pass);
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::pass);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::fail_flaky);
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::abort);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::fail_asserted);
    });

    // Recursion
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::fail);
        ct::utils::dynamic_assert(result.m_observations.size() == 2ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::pass); // the internal throw
        ct::utils::dynamic_assert(result.m_observations[1].m_status == State::fail); // the external throw
    });

    // Explicit observer
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::pass);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::pass);
    });
    test([](auto const & result) {
        ct::utils::dynamic_assert(result.m_status == Outcome::fail);
        ct::utils::dynamic_assert(result.m_observations.size() == 1ul);
        ct::utils::dynamic_assert(result.m_observations[0].m_status == State::fail);
    });
}
