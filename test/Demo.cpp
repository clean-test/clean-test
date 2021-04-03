// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include <clean-test/clean-test.h>

constexpr auto sum(auto... vs) { return (0 + ... + vs); }

namespace ct = clean_test;
using namespace ct::literals;

auto const suite = ct::Suite{"sum", [] {
    "0"_test = [] { ct::expect(sum() == 0_i); };
    "3"_test = [] { ct::expect(sum(1, 2) == 1 + 2_i); };
    "A"_test = [] { ct::expect(sum(-1) < 0_i and sum(+1) > 0_i); };
}};

int main(int argc, char ** argv)
{
    return ct::main(argc, argv);
}
