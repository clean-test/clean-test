// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "NameFilter.h"

#include <framework/Name.h>

#include <algorithm>
#include <optional>

namespace clean_test::execute {

class NameFilter::Matcher {
    static constexpr auto flags = std::regex_constants::ECMAScript | std::regex_constants::nosubs
                                | std::regex_constants::optimize;

public:
    Matcher(NameFilterToggle toggle, NameFilterProperty property, std::string_view pattern) :
        m_on_match{toggle}, m_property{property}, m_regex{pattern.cbegin(), pattern.cend(), flags}
    {}

    std::optional<NameFilterToggle> operator()(framework::Name const & name) const
    {
        if (matches(name)) {
            return m_on_match;
        }
        return {};
    }

private:
    bool matches(framework::Name const & name) const
    {
        switch (m_property) {
            case NameFilterProperty::path:
                return matches(name.path());
            case NameFilterProperty::tag:
                return matches(name.tags());
            case NameFilterProperty::any:
                return matches(name.path()) or matches(name.tags());
            default:
                std::terminate();
        }
    }

    bool matches(std::string_view const subject) const
    {
        return std::regex_search(subject.cbegin(), subject.cend(), m_regex);
    }

    bool matches(framework::Name::Tags const & tags) const
    {
        return std::any_of(
            tags.cbegin(), tags.cend(), [this](framework::Tag const & tag) { return matches(std::string_view{tag}); });
    }

    NameFilterToggle m_on_match;
    NameFilterProperty m_property;
    std::regex m_regex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NameFilter::NameFilter() noexcept = default;
NameFilter::NameFilter(NameFilter const &) noexcept = default;
NameFilter::NameFilter(NameFilter &&) noexcept = default;
NameFilter & NameFilter::operator=(NameFilter const &) noexcept = default;
NameFilter & NameFilter::operator=(NameFilter &&) noexcept = default;
NameFilter::~NameFilter() = default;

NameFilter::NameFilter(std::vector<NameFilterSetting> const & settings)
{
    for (auto const & s : settings) {
        add(s.m_toggle, s.m_property, s.m_pattern);
    }
}

NameFilterToggle NameFilter::operator()(framework::Name const & name) const
{
    for (auto const & matcher : m_matchers) {
        if (auto const toggle = matcher(name); toggle) {
            return *toggle;
        }
    }
    return m_default;
}

void NameFilter::add(NameFilterToggle toggle, NameFilterProperty property, std::string_view pattern)
{
    m_matchers.emplace_back(toggle, property, pattern);
    m_default = NameFilterToggle{not static_cast<bool>(toggle)};
}

}
