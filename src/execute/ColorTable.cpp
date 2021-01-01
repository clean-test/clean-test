// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "ColorTable.h"

namespace clean_test::execute {

ColorTable::ColoredText::ColoredText(ColorTable const & table, Color const text_color, std::string_view const text) :
    m_table{table}, m_text_color{text_color}, m_text{text}
{}

std::ostream & operator<<(std::ostream & out, ColorTable::ColoredText const & input)
{
    return out << input.m_table[input.m_text_color] << input.m_text << input.m_table[Color::off];
}

ColorTable::ColoredText ColorTable::colored(Color const color, std::string_view const text) const noexcept
{
    return ColoredText{*this, color, text};
}

}
