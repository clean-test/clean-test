// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <execute/ColorTable.h>

#include <iosfwd>

namespace clean_test::execute {

/// Utility for displaying a commandline help to users.
class HelpDisplay{
public:
    /// Output operator: Write configured @p help into @p out.
    friend std::ostream & operator<<(std::ostream & out, HelpDisplay const & help);

    ColorTable const & m_colors; //!< colors for highlighting certain parts of the help message
};

}
