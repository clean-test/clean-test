// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <string>

namespace clean_test::execute {

/// Configuration knob to denote whether a given test-case should be executed.
enum class NameFilterToggle : bool {
    disabled = false, //!< nope, don't run it
    enabled = true,  //!< yup, it's about time to execute it
};

/// Member(s) of a @c Name looked at during matching.
enum class NameFilterProperty {
    path, //!< consider path of the test-case
    tag, //!< consider (all) tags of the test-case
    any, //!< consider both of the above types of @c Property
};

/// Building block for configuration of NameFilters: Specify criterion and what to when it is met.
class NameFilterSetting {
public:
    NameFilterToggle m_toggle; //!< Whether filter specifies an inclusion or an exclusion criterion
    NameFilterProperty m_property; //!< Which components of a @c Name should be matched against
    std::string m_pattern; //!< Pattern used for matching

    friend auto operator<=>(NameFilterSetting const &, NameFilterSetting const &) = default;
};

}
