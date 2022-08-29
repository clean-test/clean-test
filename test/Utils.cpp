// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <utils/Printable.h>

#include <vector>

namespace {

namespace ct = clean_test;

// Printable ///////////////////////////////////////////////////////////////////////////////////////////////////////////

static_assert(ct::utils::Printable<int>);
static_assert(ct::utils::Printable<int &>);
static_assert(ct::utils::Printable<int const &>);
static_assert(ct::utils::Printable<int &&>);
static_assert(not ct::utils::Printable<std::vector<int>>);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{}
