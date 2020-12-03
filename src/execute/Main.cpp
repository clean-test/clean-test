// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "execute/Main.h"

#include "execute/CaseReporter.h"
#include <execute/Configuration.h>

#include <framework/Registry.h>
#include <execute/Observer.h>

#include <iostream>

namespace clean_test::execute {

int main(int argc, char ** argv)
{
    return main(Configuration::parse(argc, argv));
}

int main(Configuration const &)
{
    auto const & registry = framework::registry();
    std::cout << "Running " << std::size(registry) << " test-cases.\n";
    for (auto & tc: registry) {
        std::cout << " -> " << tc.m_name.path() << "\n";
        auto reporter = execute::CaseReporter{std::cout};
        auto observer = execute::Observer{reporter};
        try {
            tc.m_runner(observer);
        } catch (...) {
        }
        // observer -> tc.result
    }
    return 0; // TODO return number of failed tests
}

}
