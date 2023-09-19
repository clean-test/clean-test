// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstdint>
#include <string_view>
#include <cstdint>

namespace clean_test::utils::utf8 {

/// Details about a UTF-8 code-point.
class CodePoint {
public:
    bool m_is_valid; //!< whether given bytes are valid UTF-8.
    std::string_view m_bytes; //!< associated range of bytes.

    /// Load first code-point of @p data: analyze validity and determine associated number of bytes (between 1 and 4).
    static CodePoint front(std::string_view data);
};

enum class Analysis : std::uint8_t {
    plain = 0, //!< input does not necessitate any escaping whatsoever.
    valid = 1, //!< input is valid utf-8, but profits from escaped displaying still.
    invalid = 2, //!< input contains invalid utf-8 and thus can only be printed escaped.
};

/// Write @p input into @p out while making sure to discard any invalid utf-8 byte-sequences.
///
/// Skips any invalid @c CodePoint s. The returned @p Analysis describes how the entire @p input has been written.
/// In case non-@c invalid is returned, no bytes from @p input have been discarded.
Analysis analyzing_write(std::ostream & out, std::string_view input);

/// Utility wrapper for printing any byte-sequence as valid ASCII sequence utilizing escaped notation.
///
/// Example: \u0401 becomes "\xD0\x81".
class Escaped {
public:
    /// Central output operator
    ///
    /// The byte-sequence written to @p out is guaranteed to be valid utf-8 -- particularly in case @p data itself
    /// actually is invalid utf-8. The output contains enclosing double quotes. The output is an equivalent ASCII
    /// version of the input bytes that could be copied into C++ source code.
    friend std::ostream & operator<<(std::ostream & out, Escaped const & data);

    std::string_view m_data; //!< the wrapped byte-sequence (not necessarily valid utf-8).
};

}
