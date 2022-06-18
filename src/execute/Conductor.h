// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Outcome.h"

#include <execute/BufferingMode.h>

#include <vector>

namespace clean_test::framework {
class Case;
}
namespace clean_test::execute {
class NameFilter;
class ColorTable;

/// High level test-case execution orchestration facility.
class Conductor {
public:
    class Setup {
    public:
        ColorTable const & m_colors; //!< coloring details for console output.
        /// Number of worker threads for executing test-cases concurrently. Uses all available CPU cores if set to 0.
        std::size_t m_num_workers;
        BufferingMode m_buffering; //!< how test observation output is buffered.
        NameFilter const & m_filter; //!< which tests should be executed and which should be skipped.
    };

    /// Detailed c'tor: Honor all specified @p setup details.
    Conductor(Setup const & setup) noexcept;

    /// Convenience c'tor for internal tests: use automatic coloring, buffering as well as all threads and tests.
    Conductor() noexcept;

    /// Invoke all tests and return collected results.
    Outcome run() const;

private:
    /// Output final summary about passed and failed results in @p outcome including total wall time.
    void report(Outcome const & outcome) const;
    /// Print warning for @p cases registered late.
    void display_late_registration_warning(std::vector<framework::Case> const & cases) const;

    Setup const m_setup;
};

}
