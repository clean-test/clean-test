// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "TestUtilities.h"

#include <clean-test/utils/ScopeGuard.h>

#include <cstdlib>
#include <functional>

namespace {

template <typename T>
void discard(T && t)
{
    static_cast<void>(t);
}

template <typename T>
using ScopeGuard = clean_test::utils::ScopeGuard<T>;

/// Helper type with configurable con- and destructability as well as invocability.
///
/// In many implementations, @c is_nothrow_constructible also checks if the destructor throws. Thus we only offer the
/// joint template argument @tparam make.
template <bool make, bool invoke>
class Chameleon {
public:
    constexpr Chameleon() noexcept(make){}
    constexpr Chameleon(Chameleon const &) noexcept(make) {}
    constexpr Chameleon(Chameleon &&) noexcept(make) {}

    constexpr ~Chameleon() noexcept(make) {}

    constexpr void operator()() noexcept(invoke) {}
};

/// Assignable test function incrementing some value upon invocation.
///
/// Can not be implemented as lambda, since this by definition destroys copy-assignability.
class Incrementer {
public:
    constexpr explicit Incrementer(std::size_t & value) : m_value{value}
    {}

    constexpr void operator()() noexcept
    {
        ++m_value;
    }

private:
    std::reference_wrapper<std::size_t> m_value;
};

// Tests ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <bool make, bool invoke>
constexpr inline auto is_guard_nothrow_constructible
    = std::is_nothrow_constructible_v<ScopeGuard<Chameleon<make, invoke>>, Chameleon<make, invoke>>;
template <bool make, bool invoke>
constexpr inline auto is_guard_nothrow_destructible
    = std::is_nothrow_destructible_v<ScopeGuard<Chameleon<make, invoke>>>;

static_assert(std::is_nothrow_constructible_v<Chameleon<true, false>>);
static_assert(not std::is_nothrow_constructible_v<Chameleon<false, false>>);

static_assert(is_guard_nothrow_constructible<true, true>);
static_assert(not is_guard_nothrow_constructible<false, false>);
static_assert(not is_guard_nothrow_constructible<true, false>);
static_assert(not is_guard_nothrow_constructible<false, true>);

static_assert(is_guard_nothrow_destructible<true, true>);
static_assert(not is_guard_nothrow_destructible<true, false>);
static_assert(not is_guard_nothrow_destructible<false, true>);
static_assert(not is_guard_nothrow_destructible<false, false>);

[[maybe_unused]] constexpr auto default_constructible = [] {};
static_assert(std::is_default_constructible_v<decltype(default_constructible)>);
static_assert(std::is_default_constructible_v<ScopeGuard<decltype(default_constructible)>>);
[[maybe_unused]] constexpr auto nondefault_constructible = [ref = std::ref(default_constructible)]{ discard(ref); };
static_assert(not std::is_default_constructible_v<decltype(nondefault_constructible)>);
static_assert(not std::is_default_constructible_v<ScopeGuard<decltype(nondefault_constructible)>>);

class Explicit {
public:
    explicit(true) Explicit(int)
    {}
    void operator()()
    {}
};
class Implicit {
public:
    explicit(false) Implicit(int)
    {}
    void operator()()
    {}
};
static_assert(std::is_convertible_v<int, Implicit>);
static_assert(std::is_convertible_v<int, clean_test::utils::ScopeGuard<Implicit>>);
static_assert(not std::is_convertible_v<int, Explicit>);
static_assert(not std::is_convertible_v<int, clean_test::utils::ScopeGuard<Explicit>>);

void test_move_assign()
{
    auto identity = [](auto && x) {
        // Circumvent warnings about move-assignment to self.
        return std::forward<decltype(x)>(x);
    };

    auto cnt0 = 0ul;
    auto cnt1 = 0ul;
    {
        auto sg0 = clean_test::utils::ScopeGuard{Incrementer{cnt0}};
        auto sg1 = clean_test::utils::ScopeGuard{Incrementer{cnt1}};

        // regular move-assignment
        sg0 = std::move(sg1);
        clean_test::utils::dynamic_assert(cnt0 == 1ul);
        clean_test::utils::dynamic_assert(cnt1 == 0ul);

        // self move-assignment
        sg0 = identity(std::move(sg0));
        clean_test::utils::dynamic_assert(cnt0 == 1ul);
        clean_test::utils::dynamic_assert(cnt1 == 0ul);
    }
    clean_test::utils::dynamic_assert(cnt0 == 1ul);
    clean_test::utils::dynamic_assert(cnt1 == 1ul);
}

void test_dismiss()
{
    auto cnt = 0ul;
    {
        auto sg = clean_test::utils::ScopeGuard{[&cnt] { ++cnt; }};
        sg.dismiss();
        clean_test::utils::dynamic_assert(cnt == 0ul);
    }
    clean_test::utils::dynamic_assert(cnt == 0ul);
}

}

int main()
{
    // How helpful would a framework be, where tests are registered automagically xD. Oh my...
    test_move_assign();
    test_dismiss();
}
