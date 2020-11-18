// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include <clean-test/framework.h>

#include <iostream>

namespace {

namespace ct = clean_test;
using namespace ct::literals;

// Registration (obviously) is supported from across multiple translation units.
auto const suite = ct::Suite{"second_tu", []() {
    "case"_test = [] {};
}};

}
