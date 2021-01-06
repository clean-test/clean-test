// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "CaseResult.h"
#include "CaseStatus.h"
#include "Observation.h"

#include <array>
#include <iosfwd>
#include <string_view>

namespace clean_test::execute {

class ColorTable;

/// Output facility for events encountered during test-execution.
///
/// Supports configurable verbosity depending on the severity of the observed event status.
class CaseReporter {
public:
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

    /// Detailed c'tor: store @p output (don't output anything) and @p colors.
    CaseReporter(std::ostream & output, ColorTable const & colors);

    /// Report @c Start -event.
    void operator()(Start const &);

    /// Report @p observation details (iff its state is enabled).
    void operator()(Observation const & observation);

    /// Report @c Stop -event.
    void operator()(Stop const &);

private:
    /// Load synchronizing facility for writing output.
    auto output() const;

    std::ostream & m_output; //!< stream to write output to.
    ColorTable const & m_colors; //!< coloring details for output badges.
    /// Filter configuration for different output status.
    std::array<bool, ObservationStatus::num_values> const m_is_observation_state_enabled;
};

}
