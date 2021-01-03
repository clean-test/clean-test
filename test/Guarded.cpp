// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <utils/Guarded.h>

#include <future>
#include <mutex>
#include <vector>

namespace {

void base()
{
    using G = clean_test::utils::Guarded<std::string>;

    auto a = G{'a', 'b', 'c'};
    auto b = G{'x', 'y', 'z'};
    a = "huhu";
    a = std::move(b);
    clean_test::utils::discard(*a.guard());
    static_assert(std::is_assignable_v<decltype(*a.guard()), std::string>);
    clean_test::utils::discard(*static_cast<G const &>(a).guard());
    static_assert(not std::is_assignable_v<decltype(*static_cast<G const &>(a).guard()), std::string>);
}

void base_const()
{
    auto const a = clean_test::utils::Guarded<int const>{17};
    clean_test::utils::discard(*a.guard());
    static_assert(not std::is_assignable_v<decltype(*a.guard()), int>);
}

void mutex_reference()
{
    auto mtx = std::mutex{};
    auto max = clean_test::utils::Guarded{mtx, 0ul};
    clean_test::utils::discard(max);
}

void parallel()
{
    static constexpr auto num_threads = 128ul;
    auto max = clean_test::utils::Guarded{0ul};

    auto incrementers = std::vector<std::future<void>>{};
    while (incrementers.size() < num_threads) {
        incrementers.emplace_back(std::async([&] {
            auto g = max.guard();
            *g += 1ul;
        }));
    }

    for (auto & increment : incrementers) {
        increment.wait();
    }

    clean_test::utils::dynamic_assert(*max.guard() == num_threads);
}

}

int main()
{
    base();
    base_const();
    mutex_reference();
    parallel();
}
