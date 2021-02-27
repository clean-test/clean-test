// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "framework/ExpectationObserver.h"

#include "execute/Abortion.h"
#include "execute/ObservationStatus.h"
#include "execute/Observer.h"

#include "utils/UTF8Encoder.h"

namespace clean_test::framework {
namespace {

using Status = execute::ObservationStatus;

Status status(bool const failed, bool const flaky, bool const asserted)
{
    if (not failed) {
        return Status::pass;
    }
    if (asserted) {
        return Status::fail_asserted;
    }
    if (flaky) {
        return Status::fail_flaky;
    }
    return Status::fail;
}

}

ExpectationObserver::~ExpectationObserver() noexcept(false)
{
    m_what = utils::UTF8Encoder::sanitize(m_what);
    auto buffered = utils::UTF8Encoder::sanitize(std::move(m_buffer).str());
    m_observer.get()(
        {m_where, status(m_failed, m_may_fail, m_abort_on_failure), std::move(m_what), std::move(buffered)});
    if (m_failed and m_abort_on_failure) {
        throw execute::Abortion{};
    }
}

}
