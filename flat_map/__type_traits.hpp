// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

#include "flat_map/__fwd.hpp"

namespace flat_map::detail
{

template <typename T, typename = void>
struct is_allocator : public std::false_type {};

template <typename T>
struct is_allocator<T, std::void_t<typename T::value_type, decltype(std::declval<T&>().allocate(std::size_t{}))>> : public std::true_type {};

template <typename T>
inline constexpr bool is_allocator_v = is_allocator<T>{};

template <typename InputIterator>
using iter_key_t = std::remove_const_t<typename std::iterator_traits<InputIterator>::value_type::first_type>;

template <typename InputIterator>
using iter_val_t = typename std::iterator_traits<InputIterator>::value_type::second_type;

// Use variadics for deducing defaulted allocator
template <typename InputIterator, typename... Args>
using iter_cont_t = std::vector<std::pair<iter_key_t<InputIterator>, iter_val_t<InputIterator>>, Args...>;

template <typename T>
using remove_cvref = std::remove_reference<std::remove_cv_t<T>>;

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T, typename U> struct copy_cv { using type = T; };
template <typename T, typename U> struct copy_cv<T, U const> { using type = T const; };
template <typename T, typename U> struct copy_cv<T, U volatile> { using type = T volatile; };
template <typename T, typename U> struct copy_cv<T, U const volatile> { using type = T const volatile; };

template <typename T, typename U> using copy_cv_t = typename copy_cv<T, U>::type;

} // namespace flat_map::detail
