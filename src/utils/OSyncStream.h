// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#if defined(__cpp_lib_syncbuf) and __cpp_lib_syncbuf >= 201803
# include <syncstream>
#else
# include <utils/Guarded.h>
# include <sstream>
#endif

namespace clean_test::utils {

#if defined(__cpp_lib_syncbuf) and __cpp_lib_syncbuf >= 201803
using OSyncStream = std::osyncstream;
#else

/// Poor-man's @c std::osyncstream strawman.
class OSyncStream {
public:
    explicit OSyncStream(std::ostream & os);
    ~OSyncStream() noexcept(false);

    // movable
    OSyncStream(OSyncStream && that) noexcept = default;
    OSyncStream & operator=(OSyncStream && that) noexcept = default;

    // not copyable
    OSyncStream(OSyncStream const &) = delete;
    OSyncStream & operator=(OSyncStream const &) = delete;

    /// Output operator for generic @p value s.
    OSyncStream & operator<<(auto && value)
    {
        m_buffer << std::forward<decltype(value)>(value);
        return *this;
    }

    /// Support for manipulators like @c std::endl etc.
    OSyncStream & operator<<(std::ostream & (*manipulate)(std::ostream &))
    {
        manipulate(m_buffer);
        return *this;
    }

    /// Flush local buffer into destination stream.
    void emit();

private:
    Guarded<std::streambuf *, std::mutex &> m_destination;
    std::ostringstream m_buffer;
};

#endif

}
