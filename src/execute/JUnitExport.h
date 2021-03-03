// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Outcome.h"

#include <iosfwd>

namespace clean_test::execute {

/// XML serializable wrapper about test-execution results.
class JUnitExport {
public:
    /// Write XML representation of @p data into @p out.
    friend std::ostream & operator<<(std::ostream & out, JUnitExport data);

    Outcome const & m_outcome;
};

}
