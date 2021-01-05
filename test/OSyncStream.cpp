// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <utils/OSyncStream.h>

#include <charconv>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


namespace {

void base()
{
    auto destination = std::ostringstream{};
    auto a = clean_test::utils::OSyncStream{destination};

    a << 17 << std::endl;
    clean_test::utils::dynamic_assert(destination.str().empty());
    a.emit();
    clean_test::utils::dynamic_assert(destination.str() == "17\n");
}

void parallel()
{
    static constexpr auto sep = '#';
    static auto const num_threads = std::max(std::thread::hardware_concurrency(), 1u) * 8ul;

    // Write a bunch of large (long) numbers in parallel.
    auto destination = std::ostringstream{};
    auto writers = std::vector<std::future<void>>{};
    while (writers.size() < num_threads) {
        writers.emplace_back(std::async([&, idx = writers.size()] {
            for (auto i = 0ul; i < num_threads; ++i) {
                auto out = clean_test::utils::OSyncStream{destination};
                out << std::numeric_limits<std::size_t>::max() - idx << sep;
                // implicit emit at scope exit
            }
        }));
    }

    for (auto & write : writers) {
        write.wait();
    }

    // Extract numbers from destination
    auto const written = destination.str();
    auto view = std::string_view{written};
    auto numbers = std::vector<std::size_t>{};
    while (not view.empty()) {
        auto const end = view.find(sep);
        auto const number_view = view.substr(0, end);
        auto number = 0ul;
        auto const [pos, ec] = std::from_chars(number_view.begin(), number_view.end(), number);
        clean_test::utils::dynamic_assert(ec == std::errc{});
        numbers.emplace_back(number);
        view.remove_prefix(end + 1);
    }

    // Assert expected numbers have been written cleanly
    std::sort(numbers.begin(), numbers.end());
    for (auto i = 0ul; i < numbers.size(); i += num_threads) {
        for (auto j = i; j < i + num_threads; ++j) {
            clean_test::utils::dynamic_assert(
                numbers[j] == std::numeric_limits<std::size_t>::max() - num_threads + (i / num_threads) + 1);
        }
    }
    while (not numbers.empty()) {
        numbers.pop_back();
    }
}

}

int main()
{
    base();
    parallel();
}
