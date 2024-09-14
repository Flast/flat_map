// Copyright (c) 2021,2024 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <utility>

namespace flat_map::concepts
{

namespace detail
{

template <typename...> using bool_t = bool;

#define FLAT_MAP_DEFINE_CONCEPT(Name, T, Params, ...) \
template <typename> struct Name ## _body; \
template <typename R, typename... Args> struct Name ## _body<R(Args...)> \
{ \
      template <typename T> static constexpr auto check_impl Params -> decltype(__VA_ARGS__); \
      template <typename D = R> static constexpr detail::bool_t<decltype(check_impl<D>(std::declval<Args>()...))> check(int) { return true; } \
      template <typename = R> static constexpr bool check(...) { return false; } \
}; \
template <typename T> inline constexpr bool Name = Name ## _body<T Params>::check(0)

} // namespace flat_map::detail

FLAT_MAP_DEFINE_CONCEPT(Reservable, T, (T c, size_t n), c.reserve(n));

} // namespace flat_map::concepts
