// Copyright (c) m8mble 2022.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <tuple>
#include <utility>

namespace clean_test::utils {
namespace tuple_details {

template<class T, std::size_t n>
concept TupleElement =
    requires(T t) {
        typename std::tuple_element_t<n, std::remove_const_t<T>>;
        { get<n>(t) } -> std::convertible_to<const std::tuple_element_t<n, T>&>;
    };

template <typename T>
constexpr auto all_tuple_elements
    = []<std::size_t... idx>(std::index_sequence<idx...>) constexpr
    {
        return (TupleElement<T, idx> && ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>());

}

/// Detects tuple-like objects, i.e. types providing expected functionality in terms of std::tuple_element and _size.
template <typename T>
concept Tuple
    = requires(T t) {
          { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
          tuple_details::all_tuple_elements<T>;
      };

}
