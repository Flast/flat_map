// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <cstddef>
#include <type_traits>

namespace flat_map::detail
{

template <typename T, typename = void>
struct is_allocator : public std::false_type {};

template <typename T>
struct is_allocator<T, std::void_t<typename T::value_type, decltype(std::declval<T&>().allocate(std::size_t{}))>> : public std::true_type {};

template <typename T>
inline constexpr bool is_allocator_v = is_allocator<T>{};

} // namespace flat_map::detail
