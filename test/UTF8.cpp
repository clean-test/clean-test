// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include "utils/UTF8Encoder.h"
#include "utils/UTF8Utils.h"

#include <sstream>
#include <string_view>
#include <tuple>

namespace {

using namespace clean_test::utils;
using namespace std::string_view_literals;

void test_code_point()
{
    auto details = [](std::string_view const input) {
        auto const code_point = utf8::CodePoint::front(input);
        return std::tuple{code_point.m_is_valid, code_point.m_bytes.size()};
    };
    auto valid = [](std::size_t const l) { return std::tuple{true, l}; };
    auto invalid = [](std::size_t const l) { return std::tuple{false, l}; };

    // valid cases
    dynamic_assert(valid(0) == details(""));
    dynamic_assert(valid(0) == details({}));
    dynamic_assert(valid(1) == details("ascii"));
    dynamic_assert(valid(2) == details("\u0401")); // Cyrillic Capital Letter Io
    dynamic_assert(valid(3) == details("\u20AC")); // EUR sign
    dynamic_assert(valid(4) == details("\U0001F600")); // Grinning Face Emoji

    // invalid unicode
    dynamic_assert(invalid(1) == details("\x80")); // continuation bytes
    dynamic_assert(invalid(1) == details("\xBF"));
    dynamic_assert(invalid(1) == details("\xFE"));
    dynamic_assert(invalid(1) == details("\xFF"));

    // more bytes expected than actually given
    dynamic_assert(invalid(1) == details("\xD0")); // Cyrillic Capital Letter Io without last byte
    dynamic_assert(invalid(2) == details("\xE2\x82")); // EUR sign without last byte
    dynamic_assert(invalid(3) == details("\xF0\x9F\x98")); // Grinning Face Emoji without last byte
    dynamic_assert(invalid(4) == details("\xF0\x82\x82\xAC")); // EUR sign in overlong representation

    // overlong form
    dynamic_assert(invalid(2) == details("\xC0\x80"));
    dynamic_assert(invalid(3) == details("\xE0\x80\x80"));
    dynamic_assert(invalid(4) == details("\xF0\x80\x80\x80"));
    dynamic_assert(invalid(2) == details("\xC1\xBF"));
    dynamic_assert(invalid(3) == details("\xE0\x9F\xBF"));
    dynamic_assert(invalid(4) == details("\xF0\x8F\xBF\xBF"));

    // utf-16 surrogate halves
    dynamic_assert(valid(3) == details("\uD7FF")); // \xED\x9F\x80
    dynamic_assert(invalid(3) == details("\xED\xA0\x80")); // forbidden U+D800
    dynamic_assert(invalid(3) == details("\xED\xBF\xBF")); // forbidden U+DFFF
    dynamic_assert(valid(3) == details("\uE000")); // \xEE\x80\x80

    // unicode range
    dynamic_assert(valid(4) == details("\U0010FFFF"));
    dynamic_assert(invalid(4) == details("\xF4\x90\x80\x80")); // U+110000 (which doesn't exist)
}

void test_analysis()
{
    using A = utf8::Analysis;

    auto const analyzed = [](std::string_view const input) {
        auto buffer = std::ostringstream{};
        auto const analysis = utf8::analyzing_write(buffer, input);
        return std::tuple{analysis, std::move(buffer).str()};
    };
    using Expected = std::tuple<A, std::string_view>;

    dynamic_assert(analyzed("ascii") == Expected{A::plain, "ascii"});
    dynamic_assert(analyzed("embedded\x80" "continuation") == Expected{A::invalid, "embeddedcontinuation"});
    dynamic_assert(analyzed("multi\nline\tcontent") == Expected{A::valid, "multi\nline\tcontent"});
    dynamic_assert(analyzed("\U0001F600") == Expected{A::valid, "\U0001F600"});
}

void test_escape()
{
    auto const escaped = [](std::string_view input) {
        auto buffer = std::ostringstream{};
        buffer << utf8::Escaped{input};
        return std::move(buffer).str();
    };

    // valid cases
    dynamic_assert(escaped("ascii") == R"R("ascii")R");
    dynamic_assert(escaped("\u0401") == R"R("\xD0\x81")R"sv); // Cyrillic Capital Letter Io
    dynamic_assert(escaped("\u20AC") == R"R("\xE2\x82\xAC")R"sv); // EUR sign
    dynamic_assert(escaped("\U0001F600") == R"R("\xF0\x9F\x98\x80")R"sv); // Grinning Face Emoji
    dynamic_assert(escaped("multi\nline\tcontent") == R"R("multi\nline\tcontent")R"); // escape sequences

    // invalid unicode
    dynamic_assert(
        escaped("embedded\x80"
                "continuation")
        == R"R("embedded\x80" "continuation")R");
    dynamic_assert(escaped("embedded\x80invalid") == R"R("embedded\x80invalid")R");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void test_sanitize()
{
    // valid input
    dynamic_assert(UTF8Encoder::sanitize("ascii") == "ascii");
    dynamic_assert(UTF8Encoder::sanitize("\u0401") == "\u0401");
    dynamic_assert(UTF8Encoder::sanitize("ascii \u0401") == "ascii \u0401");

    // invalid input
    dynamic_assert(UTF8Encoder::sanitize("ascii X\x80Y \u0401") == "ascii XY (\"X\\x80Y\") \u0401");
    dynamic_assert(UTF8Encoder::sanitize("a \"X\x80Y\" b") == R"R(a "XY ("X\x80Y")" b)R");

    // corner cases
    dynamic_assert(UTF8Encoder::sanitize("").empty());
    dynamic_assert(UTF8Encoder::sanitize("\"") == "\"");
    dynamic_assert(UTF8Encoder::sanitize(" ") == " ");
    dynamic_assert(UTF8Encoder::sanitize("foo\"") == "foo\"");
}

void test_elaborate()
{
    static auto elaborated = [](std::string_view const input) {
        auto buffer = std::ostringstream{};
        buffer << UTF8Encoder::Elaborated{input};
        return std::move(buffer).str();
    };

    dynamic_assert(elaborated("") == "\"\"");
    dynamic_assert(elaborated("ascii") == "\"ascii\"");
    dynamic_assert(elaborated("a \u0401 b") == "\"a \u0401 b\" (\"a \\xD0\\x81 b\")");
    dynamic_assert(elaborated("a \x80 b") == "\"a  b\" (invalid utf-8: \"a \\x80 b\")");
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    test_code_point();
    test_analysis();
    test_escape();
    test_sanitize();
    test_elaborate();
}
