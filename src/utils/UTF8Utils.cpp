// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "utils/UTF8Utils.h"

#include "utils/ScopeGuard.h"

#include <algorithm>
#include <ostream>
#include <cctype>

namespace clean_test::utils::utf8 {
namespace {

//auto analyze(std::string_view const input) {
//    auto const n = num_bytes(input.front());
//    if (n == 0 or n > input.size()) {
//        return std::tuple{Analysis::invalid, n};
//    }
//    for (auto i = 0u; i < n; ++i) {
//        if (not is_continuation(input[i])) {
//            return std::tuple{Analysis::invalid, n};
//        }
//    }
//    input.remove_prefix(n);
//}

/// Variant of @c std::count_if for whole containers.
template <typename Data, std::invocable<typename Data::value_type> Predicate>
constexpr bool all_of(Data const & data, Predicate && predicate)
{
#if __cpp_lib_ranges
    return std::ranges::all_of(data, std::forward<Predicate>(predicate));
#else
    return std::all_of(std::cbegin(data), std::cend(data), std::forward<Predicate>(predicate));
#endif
}

/// Invoke @p app for sub-ranges of @p data denoting utf-8 code-points.
///
/// For each invalid bytes / codepoints @p app is invoked with empty input.
void apply_to_codepoints(std::string_view data, std::invocable<CodePoint> auto app)
{
    while (not data.empty()) {
        auto const code_point = CodePoint::front(data);
        app(code_point);
        data.remove_prefix(code_point.m_bytes.size());
    }
}

constexpr bool is_continuation(std::string_view::value_type const start)
{
    auto const byte = static_cast<std::uint8_t>(start);
    return 0b1000'0000u <= byte and byte < 0b1100'0000u;
}

constexpr std::string_view escape(std::string_view::value_type const c)
{
    switch (c) {
        // single quote ignored (ok as is withing string literal)
        case '"': // double quote
            return "\\\"";
            // question mark ignored (only useful for ancient trigraphs)
        case '\\': // backslash
            return "\\\\";
        case '\a': // audible bell
            return "\\a";
        case '\b': // backspace
            return "\\b";
        case '\f': // form feed new page
            return "\\f";
        case '\n': // new line
            return "\\n";
        case '\r': // carriage return
            return "\\r";
        case '\t': // horizontal tab
            return "\\t";
        case '\v': // vertical tab
            return "\\v";
        default:
            return {};
    }
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodePoint CodePoint::front(std::string_view const data)
{
    auto generate = [&data](bool const is_a_priori_valid, std::size_t const length) constexpr -> CodePoint
    {
        auto const bytes = data.substr(0ul, length);
        return {is_a_priori_valid and bytes.size() == length and all_of(bytes.substr(1), is_continuation), bytes};
    };

    if (data.empty()) {
        return {true, {}};
    }

    auto const lead = static_cast<std::uint8_t>(data.front());
    if (lead < 0b1000'0000u) { // single byte sequence
        return generate(true, 1);
    }
    if (lead < 0b1100'0000u or data.size() == 1) { // continuation bytes or too few input bytes
        return generate(false, 1);
    }
    auto const follow = static_cast<std::uint8_t>(data[1]); // from here on: at least two-bytes.
    if (lead < 0b1110'0000u) {
        return generate(
            lead > 0b1100'0001u, // check for overlong representation of a single-byte sequence.
            2);
    }
    if (lead < 0b1111'0000u) {
        return generate(
            (lead != 0b1110'0000u or follow >= 0b1010'0000u) // shortest (i.e. not overlong form of a 2-byte)
                and (lead != 0b1110'1101u or follow < 0b1010'0000u), // outside of reserved utf-16 surrogate halves
            3);
    }
    if (lead <= 0b1111'0100u) {
        return generate(
            (lead != 0b1111'0000u or follow >= 0b1001'0000u) // shortest (i.e. not overlong form of a 3-byte)
                and (lead != 0b1111'0100u or follow < 0b1001'0000u), // within unicode limit
            4);
    }
    // larger than U+10FFFF (the unicode limit derived from utf-16).
    return generate(false, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Analysis analyzing_write(std::ostream & out, std::string_view input)
{
    auto result = Analysis::plain;
    apply_to_codepoints(input, [&out, &result](CodePoint const code_point) {
        auto const analysis = [&out, &code_point] {
            if (not code_point.m_is_valid) {
                return Analysis::invalid; // and skip printing
            }
            out << code_point.m_bytes;
            if (code_point.m_bytes.length() == 1ul and escape(code_point.m_bytes.front()).empty()) {
                return Analysis::plain;
            }
            return Analysis::valid;
        }();
        result = std::max(result, analysis);
    });
    return result;
}

std::ostream & operator<<(std::ostream & out, Escaped const & input)
{
    enum class Status {
        regular,
        escaped,
    };

    out << '"';
    auto status = Status::regular; // how the last byte has been written to out
    apply_to_codepoints(input.m_data, [&out, &status](CodePoint const code_point) {
        if (code_point.m_is_valid and code_point.m_bytes.length() == 1ul) {
            if (auto const escaped = escape(code_point.m_bytes.front()); not escaped.empty()) {
                out << escaped;
            } else {
                if (status == Status::escaped and std::isxdigit(code_point.m_bytes.front())) {
                    // We have to disambiguate e.g. \x00A and thus break the string into \x00" "A.
                    out << "\" \"";
                }
                out << code_point.m_bytes;
            }
            status = Status::regular;
            return;
        }

        auto const reset = ScopeGuard{[&out, flags = out.flags()] { out.flags(flags); }};
        out << std::hex << std::uppercase;
        for (auto const c: code_point.m_bytes) {
            out << "\\x" << static_cast<int>(static_cast<std::uint8_t>(c));
        }
        status = Status::escaped;
    });
    out << '"';
    return out;
}

}
