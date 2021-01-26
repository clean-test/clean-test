// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ColorTable.h"
#include "NameFilter.h"

#include <iosfwd>
#include <vector>

namespace clean_test::framework {
class Case;
}
namespace clean_test::execute {

/// Printable wrapper to visualize (selected) test-tree details.
class TreeDisplay {
public:
    using Input = std::vector<framework::Case>;

    class Setup {
    public:
        ColorTable const & m_colors; //!< coloring details for console output.
        NameFilter const & m_filter; //!< which tests should be listed and which should be skipped.

        /// Maximum number of expansion levels in the printed tree.
        ///
        /// The special (default) value of 0 instructs to print the tree with unlimited depth.
        std::size_t m_depth = 0;
    };

    /// Detailed c'tor: Wrap for displaying @p cases in given @p setup.
    TreeDisplay(Input const & cases, Setup setup);

    /// Print visual representation of @p data as tree into @p out; terminates entire output with newline.
    friend std::ostream & operator<<(std::ostream & out, TreeDisplay const & data);

private:
    Input const & m_cases;
    Setup const m_setup;
};

}
