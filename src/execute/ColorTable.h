// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include <array>
#include <ostream>
#include <string_view>
#include <utility>

#pragma once

namespace clean_test::execute {

/// Representation all supported colors for highlighting parts of the output.
enum class Color {
    off, //!< turn any coloring off
    good, //!< highlight something positive (likely in green)
    bad, //!< highlight something negative (likely in red)
};

/// Lookup table for handles to enable a given @c Color for subsequent output.
///
/// @note We use different @c ColorTable s depending of the output (terminal?) capabilities.
class ColorTable {
public:
    static constexpr auto size = 3ul;
    using Handle = std::string_view;
    using Data = std::array<Handle, size>;

    /// Explicit c'tor: initialize table from given @p args.
    template <typename... Args>
    constexpr explicit ColorTable(Args &&... args) noexcept : m_data{std::forward<Args>(args)...}
    {}

    /// Access handle to enable @p color.
    Handle operator[](Color const color) const noexcept
    {
        return m_data[static_cast<std::size_t>(color)];
    }

    /// Utility for printing text in a given color (resets output colors afterwards).
    class ColoredText {
    public:
        /// Detailed c'tor: store details for being able to display @p text in @p text_color using @p table.
        ColoredText(ColorTable const & table, Color text_color, std::string_view text);

        /// Output specified @p input into @p out (and reset @p out's coloring afterwards)
        friend std::ostream & operator<<(std::ostream & out, ColoredText const & input);

    private:
        ColorTable const & m_table; //!< where to retrieve coloring details from
        Color m_text_color; //!< color of the text to print
        std::string_view m_text; //!< information to be printed
    };

    /// Load wrapper for displaying @p text in @p color.
    [[nodiscard]] ColoredText colored(Color color, std::string_view text) const noexcept;

private:
    Data m_data; //!< handle for enabling respective color output
};

}
