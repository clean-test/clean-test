// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <iosfwd>
#include <string>

namespace clean_test::utils {

/// Utility for high-level utf-8 operations
class UTF8Encoder {
public:
    /// Utility wrapper for generating details about arbitrary byte-sequences (particularly invalid utf-8)
    class Elaborated {
    public:
        /// Output operator
        ///
        /// Writes @p input to @p out while skipping any invalid utf-8; adds escaped version of @p input in case
        /// input contains multi-byte utf-8 or escape sequences. Example:
        /// "a \u0401 b" will be elaborated to "a \u0401 b" ("a \xD0\x81 b").
        ///
        /// The byte-sequence written to @p out is guaranteed to be valid utf-8 -- particularly in case @p data itself
        /// actually is invalid utf-8. The output contains enclosing double quotes for the @p input and the potential
        /// escaped version. Invalid utf-8 is explicitly marked as such.
        friend std::ostream & operator<<(std::ostream & out, Elaborated const & input);

        std::string_view m_data; //!< wrapped input byte-sequence.
    };

    /// Strip and escape invalid utf-8 byte-sequences from @p input
    ///
    /// Operates on parts of @p input (splits at space and double quote as this is internally used by our framework).
    /// Simply copies valid utf-8 parts. Otherwise invalid unicode is stripped and an escaped description is added.
    /// Note: Does not escape valid, multi-byte utf-8.
    static std::string sanitize(std::string_view input);
};

}
