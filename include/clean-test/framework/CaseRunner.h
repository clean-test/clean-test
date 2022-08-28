// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "ObserverFwd.h"

#include <clean-test/utils/Pack.h>
#include <clean-test/utils/Tuple.h>

#include <ranges>
#include <tuple>
#include <type_traits>

namespace clean_test::framework {
namespace case_runner_details {

template <utils::Tuple T>
consteval auto tuple_pack()
{
    return []<std::size_t... idx>(std::index_sequence<idx...>) consteval
    {
        return utils::Pack<std::tuple_element_t<idx, T>...>{};
    }
    (std::make_index_sequence<std::tuple_size_v<T>>());
}

template <std::ranges::range T>
consteval auto range_pack()
{
    return utils::Pack<std::ranges::range_reference_t<T>>{};
}

template <typename... T>
consteval auto value_pack()
{
    if constexpr (sizeof...(T) == 0) {
        return utils::Pack<>{};
    } else if constexpr (sizeof...(T) == 1) {
        return []<typename X>(utils::Pack<X>) consteval
        {
            if constexpr (std::ranges::range<X>) {
                return range_pack<X>();
            } else if constexpr (utils::Tuple<X>) {
                return tuple_pack<X>();
            } else {
                static_assert([](auto &&...) consteval { return false; }());
            }
        }
        (utils::Pack<T...>{});
    } else {
        static_assert([](auto &&...) consteval { return false; }());
    }
}

template <typename... T, typename... Value>
constexpr bool is_invocable_with(utils::Pack<Value...>)
{
    if constexpr (sizeof...(Value) == 0) {
        return std::is_invocable_v<T...>;
    } else {
        return (std::is_invocable_v<T..., Value> && ...);
    }
}

/// Determines whether @tparam Rng value (non ref) is constructible from *iterator (might be ref qualified).
template <std::ranges::range Rng>
constexpr auto value_constructible
    = std::is_constructible_v<std::ranges::range_value_t<Rng>, std::ranges::range_reference_t<Rng>>;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Identifies types that can act as container for data samples to instantiate tests with.
template <typename T>
concept CaseData
    = utils::Tuple<T>
    or std::ranges::borrowed_range<T>
    or (std::ranges::range<T> and case_runner_details::value_constructible<T>);

/// Identifies types callable with @c Observer and (all) values contained in @tparam Data (if any).
template <typename T, typename... Data>
concept CaseRunner
    = case_runner_details::is_invocable_with<T, execute::Observer &>(case_runner_details::value_pack<Data...>());

/// Identifies types that are already a @c CaseRunner or can be transformed into one (by automatic Observer handling).
template <typename T, typename... Data>
concept GenericCaseRunner = CaseRunner<T, Data...>
                         or case_runner_details::is_invocable_with<T>(case_runner_details::value_pack<Data...>());

}
