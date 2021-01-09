// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"
#include "CaseStatus.h"
#include "Observation.h"

#include <execute/BufferingMode.h>

#include <array>
#include <iosfwd>
#include <sstream>
#include <string_view>

namespace clean_test::execute {

class ColorTable;

/// Output facility for events encountered during test-execution.
///
/// Supports configurable verbosity depending on the severity of the observed event status.
class CaseReporter {
public:
    /// Configuration options for creating a reporter.
    class Setup {
    public:
        std::ostream & m_output; //!< stream to write any output into.
        ColorTable const & m_colors; //!< output coloring details
        BufferingMode m_buffering; //!< configured buffering; controls @c m_emit_regularly.
    };

    /// Event for the beginning of a test-case execution.
    class Start {
    public:
        std::string_view m_name; //!< Full path of the executed test-case.
    };

    /// Event for the finish of a test-case execution.
    class Stop {
    public:
        std::string_view m_name; //!< Full path of the executed test-case.
        CaseResult::Duration m_wall_time; //!< Elapsed time between start and stop.
        CaseStatus m_status; //!< Overall status of the test-case execution.
    };

    /// Initialize from @p setup details (see above). C'tor won't output anything.
    explicit CaseReporter(Setup setup);

    /// Non-trivial d'tor: Flushes buffer into final output destination.
    ~CaseReporter();

    CaseReporter(CaseReporter &&) = default;
    CaseReporter& operator=(CaseReporter &&) = delete;

    CaseReporter(CaseReporter const  &) = delete;
    CaseReporter& operator=(CaseReporter const  &) = delete;

    /// Report @c Start -event.
    void operator()(Start const &);

    /// Report @p observation details (iff its state is enabled).
    void operator()(Observation const & observation);

    /// Report @c Stop -event.
    void operator()(Stop const &);

private:
    /// Load synchronizing facility for writing output.
    auto output() const;

    Setup const m_setup; //!< Output configuration.
    std::ostringstream m_buffer; //!< Intermediate storage for buffering output (iff configured).
    std::ostream & m_sink; //!< Destination of all output operations: either the buffer or output of @c setup.

    /// Filter configuration for different output status.
    std::array<bool, ObservationStatus::num_values> const m_is_observation_state_enabled;
};

}
