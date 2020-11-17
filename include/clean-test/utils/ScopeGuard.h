// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <optional>
#include <type_traits>

namespace clean_test::utils {

/// Wrapper for delaying function call until lifetime of wrapper ends.
///
/// @note Could become constexpr once http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2231r0.html gets adopted.
template <typename F>
class ScopeGuard {
private:
    static constexpr inline auto is_assign_nothrow
        = std::is_nothrow_move_assignable_v<F> and std::is_nothrow_destructible_v<F>;
    static constexpr inline auto is_invoke_nothrow = std::is_nothrow_invocable_v<F>;

public:
    /// Default c'tor
    ScopeGuard() noexcept(std::is_nothrow_default_constructible_v<F>) requires(std::is_default_constructible_v<F>) :
        m_func{F{}}
    {}

    /// Direct c'tor: initialize from perfectly forwarded @p f.
    ///
    /// Implicit since the construction of f already provides the correct (in|ex)plicitness.
    template <std::convertible_to<F> T>
    explicit(false) ScopeGuard(T && f) noexcept(std::is_nothrow_constructible_v<F, T>) :
        m_func{std::forward<decltype(f)>(f)}
    {}

    /// Move c'tor: Take over responsibility from @p other.
    ScopeGuard(ScopeGuard && other) noexcept(std::is_nothrow_constructible_v<F, F &&>) : m_func{std::move(other.m_func)}
    {
        other.dismiss();
    }

    /// D'tor: invoke @c m_func.
    ~ScopeGuard() noexcept(is_invoke_nothrow and std::is_nothrow_destructible_v<F>)
    {
        invoke();
    }

    /// Assignment: Invoke this and take over @c other 's responsibilities.
    ScopeGuard & operator=(ScopeGuard && other) noexcept(is_assign_nothrow);

    // Disable copying
    ScopeGuard(ScopeGuard const &) = delete;
    ScopeGuard & operator=(ScopeGuard const &) = delete;

    /// Release stored function i.e. make sure it won't be invoked.
    void dismiss() noexcept(std::is_nothrow_destructible_v<F>)
    {
        m_func.reset();
    }

private:
    /// Call m_func (if any).
    void invoke() noexcept(is_invoke_nothrow);

    std::optional<F> m_func; //!< callable to be eventually invoked.
};

// Explicit deduction guide in order to support the combined move and copy c'tor definition above.
template <typename T>
ScopeGuard(T) -> ScopeGuard<T>;

// Implementation //////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename F>
ScopeGuard<F> & ScopeGuard<F>::operator=(ScopeGuard && other) noexcept(is_assign_nothrow)
{
    if (this != std::addressof(other)) {
        invoke();
        m_func = std::move(other.m_func);
        other.m_func.reset();
    }
    return *this;
}

template <typename F>
void ScopeGuard<F>::invoke() noexcept(is_invoke_nothrow)
{
    if (m_func) {
        (*m_func)();
    }
}

}
