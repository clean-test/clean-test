// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ExpectationObserver.h"
#include "ObservationSetup.h"
#include "ObserverFwd.h"

#include <clean-test/utils/SourceLocation.h>

namespace clean_test::framework {

/// Express the assumption that @p expr is @c true 'ish.
///
/// Redirect runtime details about this expectation to @p observer. This form should be used in highly parallel
/// test-cases, where a default @c Observer can't be deduced from the executing thread.
template <typename T>
auto expect(
    execute::Observer & observer,
    T const & expr,
    utils::SourceLocation const & sl = utils::SourceLocation::current())
{
    return ExpectationObserver{observer, expr, sl};
}

/// Express the assumption that @p expr is @c true 'ish.
///
/// Convenience shorthand of the above using a thread-specific @c Observer that will be setup by the framework
/// during test-case execution automatically.
template <typename T>
auto expect(T const & expr, utils::SourceLocation const & sl = utils::SourceLocation::current())
{
    return expect(ObservationSetup::observer(), expr, sl);
}

}
