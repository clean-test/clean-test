// Copyright (c) m8mble 2021.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <mutex>

namespace clean_test::utils {
namespace guarded_details {

template <typename T>
concept Value = not std::is_reference_v<T>;

/// Wrapper around a mutable (value) @tparam T. Only used for mutex, thus default c'tor is sufficient.
template <typename T>
class Mutable {
public:
    Mutable() = default;

    /// Semantic trick for uniform "copy" of reference and Mutable mutexes.
    Mutable(Mutable &) : Mutable{} {}

    explicit(false) operator T &() const
    {
        return m_data;
    }

private:
    mutable T m_data;
};

/// Depending on @tparam T add @c mutable keyword (references don't need it).
template <typename T>
using MutableContainer = std::conditional_t<Value<T>, Mutable<T>, T>;

/// Scoped wrapper to ensure acquired @tparam Mutex during access to @tparam T.
template <Value T, typename Mutex>
class Guard {
public:
    /// Detailed c'tor: remember @p value and acquire @p mutex.
    Guard(Mutex & mutex, T & value) : m_scope{mutex},  m_value{value}
    {}

    /// Access underlying value (reference)
    T & operator*() const
    {
        return m_value;
    }

    /// Access underlying value (pointer)
    T * operator->() const
    {
        return std::addressof(m_value);
    }

private:
    std::lock_guard<Mutex> m_scope; //!< ensured lock
    T & m_value; //!< the wrapped object.
};

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Utility for a @tparam T -value, that can only be accessed under acquired @tparam Mutex.
///
/// The underlying value can be accessed via @c guard() which ensures appropriate locking.
///
/// @notes @tparam Mutex can also be a reference type. In that case, we utilize an already existing, externally owned
/// @tparam Mutex for locking access to the wrapped value.
template <guarded_details::Value T, typename Mutex = std::mutex>
class Guarded {
public:
    /// Mutex default c'tor: Default construct owned mutex, perfect forward value initialization.
    template <typename... Args> requires requires { T{std::declval<Args>()...}; }
    explicit(false) Guarded(Args &&... args)  : m_mutex{}, m_value{std::forward<Args>(args)...}
    {}

    /// Mutex reference c'tor: store (externally owned) mutex reference, perfect forward value initialization.
    template <typename... Args> requires requires { T{std::declval<Args>()...}; }
    Guarded(Mutex & mutex, Args &&... args) : m_mutex{mutex}, m_value{std::forward<Args>(args)...}
    {}

    // not copyable
    Guarded(Guarded const &) = delete;
    Guarded & operator=(Guarded const &) = delete;

    /// Move c'tor
    ///
    /// Intentionally skips locking on @p that, since the caller has to synchronize end of lifetime of @p that anyways.
    Guarded(Guarded && that) : m_mutex{that.mutex()}, m_value{std::move(that.m_value)}
    {}

    /// Move assignment from @p other @c Guarded.
    ///
    /// Intentionally skips locking on @p that, since the caller has to synchronize end of lifetime of @p that anyways.
    Guarded & operator=(Guarded && that) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        return *this = std::move(that.m_value);
    }

    /// Move assignment from other @p value.
    Guarded & operator=(std::convertible_to<T> auto && value) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        *guard() = std::forward<decltype(value)>(value);
        return *this;
    }

    /// Generate proxy with locked access. Returned object's lifetime controls locking of underlying mutex.
    [[nodiscard]] auto guard()
    {
        return guard_impl(*this);
    }

    /// Generate proxy with locked const access. Returned object's lifetime controls locking of underlying mutex.
    [[nodiscard]] auto guard() const
    {
        return guard_impl(*this);
    }

    /// (Unlocked) rvalue-ref release: Only intended for use in destructors.
    [[nodiscard]] auto release() &&
    {
        return std::move(m_value);
    }

private:
    /// Access underlying mutex reference.
    Mutex & mutex() const
    {
        return m_mutex;
    }

    template <typename G>
    [[nodiscard]] static auto guard_impl(G & guarded)
    {
        return guarded_details::Guard{guarded.mutex(), guarded.m_value};
    }

    guarded_details::MutableContainer<Mutex> m_mutex; //!< the mutex to lock before using value
    T m_value; //!< the protected value
};

template <guarded_details::Value T>
Guarded(T) -> Guarded<T>;

template <guarded_details::Value T, typename Mutex>
Guarded(Mutex &, T) -> Guarded<T, Mutex &>;

}
