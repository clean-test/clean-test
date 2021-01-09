// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

namespace clean_test::execute {

enum class BufferingMode {
    /// No buffering whatsoever: messages of different test-cases can appear arbitrarily interleaved.
    off,
    /// Collect all messages of a test-case and display them once it is executed completely.
    ///
    /// This mode provides improved readability at the expense of memory (for buffering). Also be aware that a crashing
    /// test-case will not display any messages.
    testcase,
};

}
