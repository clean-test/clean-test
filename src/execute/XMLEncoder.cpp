// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "XMLEncoder.h"

namespace clean_test::execute {

XMLEncoder & operator<<(XMLEncoder & encoder, std::string_view str)
{
    for (; not str.empty(); str.remove_prefix(1ul)) {
        encoder.m_output << XMLEncoder::escaped({std::addressof(str.front()), 1ul});
    }
    return encoder;
}

std::string_view XMLEncoder::escaped(std::string_view const input)
{
    switch (input.front()) {
        case '&':
            return "&amp;";
        case '"':
            return "&quot;";
        case '\'':
            return "&apos;";
        case '<':
            return "&lt;";
        case '>':
            return "&gt;";
        case '\n':
            return "&#xA;";
        default:
            return input;
    }
}

}
