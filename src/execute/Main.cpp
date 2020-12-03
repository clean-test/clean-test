// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "execute/Main.h"

#include "CaseEvaluator.h"

#include "framework/Registry.h"

#include "execute/Configuration.h"

#include <algorithm>
#include <iostream>
#include <version>

namespace clean_test::execute {
namespace {

/// Variant of @c std::count_if for whole containers.
template <typename Data, std::invocable<typename Data::value_type> Predicate>
constexpr std::size_t count_if(Data const & data, Predicate && predicate)
{
#if __cpp_lib_ranges
    return std::ranges::count_if(data, std::forward<Predicate>(predicate));
#else
    return std::count_if(std::cbegin(data), std::cend(data), std::forward<Predicate>(predicate));
#endif
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
    return main(Configuration::parse(argc, argv));
}

int main(Configuration const &)
{
    auto & registry = framework::registry();
    std::cout << "Running " << std::size(registry) << " test-cases.\n";
    auto results = std::vector<CaseResult>{};
    for (auto & tc: registry) {
        results.emplace_back(CaseEvaluator{}(tc));
        tc.m_runner = nullptr;
    }
    return count_if(results, [](auto const & r) { return r.m_status != CaseStatus::pass; });
}

}
