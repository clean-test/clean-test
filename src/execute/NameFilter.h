// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <execute/NameFilterSetting.h>

#include <regex>
#include <string_view>
#include <vector>

namespace clean_test::framework {
class Name;
}
namespace clean_test::execute {

/// Configurable selector for test-case names to selectively en- and disable test-cases at runtime.
///
/// Such a filter is composed of multiple in- and exclusion criteria that are considered one after another.
/// If a defined pattern matches the defined property of a @c Name, the associated @c NameFilterToggle is returned
/// (c.f. @c ::add). If no match is found, the opposite @c NameFilterToggle of the last added configuration is used.
///
/// Example: Assume one added filter with "^a"-pattern on @c NameFilterProperty::path with @c NameFilterToggle::enabled.
/// This disables all cases but those whose @c Name 's path starts with "a".
class NameFilter {
public:
    // Explicitly default all c'tors / d'tor to avoid instantiation of the @c std::vector with fwd declared Matcher.
    NameFilter() noexcept;
    NameFilter(NameFilter const &) noexcept;
    NameFilter(NameFilter &&) noexcept;
    NameFilter & operator=(NameFilter const &) noexcept;
    NameFilter & operator=(NameFilter &&) noexcept;
    ~NameFilter();

    /// Detailed c'tor: Add all @p settings (c.f. @c ::add below).
    explicit NameFilter(std::vector<NameFilterSetting> const & settings);

    /// Determine @c NameFilterToggle for @p name (see above class description).
    ///
    /// Can be used concurrently from multiple threads without further synchronization.
    NameFilterToggle operator()(framework::Name const & name) const;

    /// Advise to return @p toggle when @p pattern matches @p property of a name.
    void add(NameFilterToggle toggle, NameFilterProperty property, std::string_view pattern);

private:
    class Matcher;

    NameFilterToggle m_default = NameFilterToggle::enabled;
    std::vector<Matcher> m_matchers;
};
}
