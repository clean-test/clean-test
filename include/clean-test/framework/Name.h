// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Tag.h"

#include <vector>
#include <string>

namespace clean_test::framework {

/// Common naming type for both test-suites and -cases: Manages hierarchical path as well as associated tags.
class Name {
public:
    static constexpr auto separator = std::string_view{"/"}; //!< separator for different path components

    using Tags = std::vector<Tag>;

    /// Default c'tor with empty path and tags.
    Name() = default;

    explicit Name(std::string_view const path) : m_path{path}
    {}

    explicit Name(Tag const & tag) : m_tags{tag}
    {}

    std::string_view path() const
    {
        return m_path;
    }

    Tags const & tags() const
    {
        return m_tags;
    }

    /// Extend this name's @c path() by @p path.
    Name & operator/=(std::string_view path);

    /// Add @p tag to this name's @p tags().
    Name & operator/=(Tag const & tag);

    /// Extend this name by @p other 's path and tags.
    Name & operator/=(Name const & other);

private:
    std::string m_path;
    Tags m_tags;
};

template <typename L, typename R>
Name operator/(L && l, R && r)
    requires (std::is_constructible_v<Name, L> and requires { Name{std::forward<L>(l)} /= std::forward<R>(r); })
{
    auto result = Name{std::forward<L>(l)};
    result /= std::forward<R>(r);
    return result;
}

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

inline Name & Name::operator/=(std::string_view const extension)
{
    if (not path().empty()) {
        m_path.append(separator);
    }
    m_path.append(extension);
    return *this;
}

inline Name & Name::operator/=(Tag const & tag)
{
    m_tags.emplace_back(tag);
    return *this;
}

inline Name & Name::operator/=(Name const & other)
{
    *this /= other.path();
    auto const & other_tags = other.tags();
    m_tags.insert(std::end(m_tags), std::begin(other_tags), std::end(other_tags));
    return *this;
}

}
