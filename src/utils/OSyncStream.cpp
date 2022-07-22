// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#include "OSyncStream.h"

#if not defined(__cpp_lib_syncbuf) or __cpp_lib_syncbuf < 201803

#include <ostream>
#include <unordered_map>
#include <utility>

namespace clean_test::utils {
namespace {

/// Manages buffer-specific mutex instances.
auto & buffer_mutex()
{
    static auto singleton = Guarded{std::unordered_map<std::streambuf *, std::mutex>{}};
    return singleton;
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OSyncStream::OSyncStream(std::ostream & os) :
    m_destination{(*buffer_mutex().guard())[os.rdbuf()], os.rdbuf()}, m_buffer{}
{}

OSyncStream::~OSyncStream() noexcept(false)
{
    emit();
}

void OSyncStream::emit()
{
    auto const data = std::exchange(m_buffer, {}).str();
    (**(m_destination.guard())).sputn(data.data(), data.size());
}

}
#endif
