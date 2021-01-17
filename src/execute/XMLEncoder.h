// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <ostream>
#include <string_view>
#include <type_traits>

namespace clean_test::execute {

/// Output stream wrapper with XML byte-encoding support.
///
/// XML requires certain bytes (in certain places) to be output encoded. Example: '<' => '&lt;'.
/// An XMLEncoder supports a (minimal) replacement table for proper and safe output of arbitrary input bytes.
///
/// The XML-encoding can be en- and disabled mid streaming. Example:
/// @code
///     std::cout << "<not-encoded>" << XMLEncoder::On{} << "<encoded>" << XMLEncoder::Off{} << '!';
/// @endcode
/// will print '<not-encoded>&lt;encoded&gt;!'.
class XMLEncoder {
public:
    /// Detailed c'tor: wrap @p output.
    explicit XMLEncoder(std::ostream & output) : m_output{output}
    {}

    /// Write @p str into @p encoder while honoring special XML-encoding (for special chars like '<', '&', etc.).
    friend XMLEncoder & operator<<(XMLEncoder & encoder, std::string_view str);

    /// Convenience shorthand for outputting one-byte @c std::string_view s.
    friend XMLEncoder & operator<<(XMLEncoder & encoder, char const c)
    {
        return encoder << std::string_view{std::addressof(c), 1ul};
    }

    /// Output operator for trivial, numeric types that require no escaping.
    friend XMLEncoder & operator<<(XMLEncoder & encoder, auto const num) requires(std::is_trivial_v<decltype(num)>)
    {
        encoder.m_output << num;
        return encoder;
    }

    /// Utility for enabling XML-encoding mid-output (c.f. example above).
    class On {
    public:
        friend auto operator<<(std::ostream & out, On const &)
        {
            return XMLEncoder{out};
        }
    };

    /// Utility for disabling XML-encoding mid-output (c.f. example above).
    class Off {
    public:
        friend std::ostream & operator<<(std::convertible_to<XMLEncoder> auto && encoder, Off const &)
        {
            return static_cast<XMLEncoder &>(encoder).m_output;
        }
    };

private:
    friend std::ostream & operator<<(std::convertible_to<XMLEncoder> auto && encoder, Off const &);

    /// Determine XML-encoded output for first byte of @p input.
    static std::string_view escaped(std::string_view input);

    std::ostream & m_output; //!< wrapped output stream
};

}
