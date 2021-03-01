// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "utils/UTF8Encoder.h"

#include "UTF8Utils.h"

#include <ostream>
#include <sstream>

namespace clean_test::utils {
namespace {

using namespace utf8;

/// Invoke @p app for substrings of @p data split by space and quotes.
///
/// @note These markers are used by the framework internally to separate different parts of the output.
void apply_to_splits(std::string_view const markers, std::string_view data, std::invocable<std::string_view> auto app)
{
    for (auto s = data.find_first_of(markers); s != std::string_view::npos; s = data.find_first_of(markers)) {
        if (s != 0ul) {
            app(data.substr(0ul, s));
        }
        app(data.substr(s, 1ul));
        data.remove_prefix(s + 1ul);
    }
    if (not data.empty()) {
        return app(data);
    }
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream & operator<<(std::ostream & out, UTF8Encoder::Elaborated const & e)
{
    out << '"';
    auto const analysis = analyzing_write(out, e.m_data);
    out << '"';

    switch (analysis) {
        case Analysis::plain:
            break;
        case Analysis::invalid:
            out << " (invalid utf-8: " << Escaped{e.m_data} << ')';
            break;
        case Analysis::valid:
            out << " (" << Escaped{e.m_data} << ')';
            break;
        default:
            std::terminate();
    }
    return out;
}

std::string UTF8Encoder::sanitize(std::string_view const input)
{
    // internal framework markers: the space is placed around any expression components (arithmetic operators,
    // parenthesis, etc.); the double-quotes are added in order to avoid escaping these quotes.
    static auto constexpr markers = std::string_view{" \""};

    auto buffer = std::ostringstream{};
    apply_to_splits(markers, input, [&buffer](std::string_view const part) {
        auto const analysis = analyzing_write(buffer, part);
        switch (analysis) {
            case Analysis::plain:
            case Analysis::valid:
                break; // don't do anything further; we already completed printing above.
            case Analysis::invalid:
                buffer << " (" << Escaped{part} << ')';
                break;
            default:
                std::terminate();
        }
    });
    return std::move(buffer).str();
}

}
