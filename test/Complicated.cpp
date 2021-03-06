// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <execute/CaseResult.h>
#include <execute/CaseStatus.h>
#include <execute/Conductor.h>

#include <clean-test/framework.h>

#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace {

namespace ct = clean_test;
using namespace ct::literals;

// Ensure the runner is stored correctly. If it were stored statically in a function local variable, this would only
// allow one runner per type. This case ensures, this in fact is not the case.
auto const by_type = ct::Suite{"by_type", [] {
    auto generate = [](bool const expectation) {
        return [=] { ct::expect(expectation); };
    };
    "true"_test = generate(true);
    "false"_test = generate(false);
}};

// Dynamically expanding (endless) test loop.
auto const dummy = [] {
    auto generate = [](std::size_t const num, auto generator) -> void {
        ct::Test{"recursive/" + std::to_string(num)} = [=] {
            generator(num + 1ul, generator);
        };
    };
    generate(0, generate);
    return 0;
}();

std::string_view const wrong_observer_message = "This is a message directed to the wrong observer!";
auto const wrong_observer = [] {
    "wrong"_test = [](ct::Observer &) {
        ct::expect(false) << wrong_observer_message;
    };
    return 0;
}();

// Demonstrate that expectations are thread-safe (can even fail on the same expectation concurrently)
auto const num_simultaneous_failures = std::max(std::thread::hardware_concurrency(), 1u) * 8u;
auto const simultaneous_failures = ct::Test{"simultaneous_failures", [](ct::Observer& obs) {
    constexpr auto order = std::memory_order_relaxed;
    auto go = std::condition_variable{};
    auto start_failing = std::atomic<bool>{false};

    auto fail_on_go = [&] {
        auto mtx = std::mutex{};
        auto lock = std::unique_lock{mtx};
        go.wait(lock, [&] { return start_failing.load(order); });

        ct::expect(obs, false); // this is the expectation subject to this test.
    };

    auto workers = std::vector<std::jthread>{};
    while (workers.size() < num_simultaneous_failures) {
        workers.emplace_back(fail_on_go);
    }

    start_failing.store(true, order);
    go.notify_all();
}};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    auto const results = ct::execute::Conductor{}.run().m_results;

    auto by_path = [&results] {
        auto result = std::map<std::string_view, std::size_t>{};
        auto index = 0ul;
        for (auto const & r: results) {
            result.emplace(r.m_name_path, index++);
        }
        return result;
    }();

    auto result = [&results, &by_path](std::string_view const who) -> decltype(auto) {
        auto const pos = by_path.find(who);
        ct::utils::dynamic_assert(pos != by_path.cend());
        return results[pos->second];
    };

    ct::utils::dynamic_assert(result("by_type/true").m_status == ct::execute::CaseStatus::pass);
    ct::utils::dynamic_assert(result("by_type/false").m_status == ct::execute::CaseStatus::fail);

    ct::utils::dynamic_assert(result("wrong").m_observations.empty());
    ct::utils::dynamic_assert(result("unknown").m_observations.size() == 1ul);
    ct::utils::dynamic_assert(result("unknown").m_observations.front().m_description == wrong_observer_message);

    auto const & r = result("simultaneous_failures");
    ct::utils::dynamic_assert(r.m_observations.size() == num_simultaneous_failures);
}
