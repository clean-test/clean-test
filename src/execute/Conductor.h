// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ColorTable.h"
#include "CaseResult.h"

#include <vector>

namespace clean_test::framework {
class Case;
}
namespace clean_test::execute {

/// High level test-case execution orchestration facility.
class Conductor {
public:
    using Results = std::vector<CaseResult>;

    /// Detailed c'tor
    explicit Conductor(ColorTable const & colors) noexcept;

    /// Convenience c'tor for internal tests: use automatic coloring.
    Conductor() noexcept;

    /// Invoke all tests and return collected results.
    Results run() const;

private:
    /// Output final summary about passed and failed @p results.
    void report(Results const & results) const;
    /// Print warning for @p cases registered late.
    void display_late_registration_warning(std::vector<framework::Case> const & cases) const;

    ColorTable const & m_colors; //!< coloring details for console output.
};

}
