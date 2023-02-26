// Copyright (c) 2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <tuple>
#include <utility>

#include "flat_map/__fwd.hpp"
#include "flat_map/__type_traits.hpp"

namespace flat_map::detail
{

template <std::size_t... N, typename F, typename T>
constexpr auto tuple_transform_impl(std::index_sequence<N...>, F f, T&& t)
{
    return f(std::get<N>(std::forward<T>(t))...);
}

template <std::size_t N, typename F, typename... T>
constexpr auto tuple_transform_impl2(F& f, T&&... t)
{
    return f(std::get<N>(std::forward<T>(t))...);
}

template <std::size_t... N, typename F, typename... T>
constexpr auto tuple_transform_impl(std::index_sequence<N...>, F f, T&&... t)
{
    if constexpr (std::is_void_v<decltype(tuple_transform_impl2<0>(f, std::forward<T>(t)...))>)
    {
        (tuple_transform_impl2<N>(f, std::forward<T>(t)...), ...);
    }
    else
    {
        return std::tuple{tuple_transform_impl2<N>(f, std::forward<T>(t)...)...};
    }
}

template <typename F, typename Head, typename... Tail>
constexpr auto tuple_transform(F f, Head&& head, Tail&&... tail)
{
    static_assert(((std::tuple_size_v<remove_cvref_t<Head>> == std::tuple_size_v<remove_cvref_t<Tail>>) && ...));
    using indices_t = std::make_index_sequence<std::tuple_size_v<remove_cvref_t<Head>>>;
    return tuple_transform_impl(indices_t{}, std::move(f), std::forward<Head>(head), std::forward<Tail>(tail)...);
}

#ifdef __cpp_lib_ranges_zip

// C++23 zip support (P2321)
template <typename... T>
using tuple = std::tuple<T...>;

#else

template <typename... T>
class tuple : public std::tuple<T...>
{
public:
    auto& base() noexcept { return static_cast<std::tuple<T...>&>(*this); }
    auto& base() const noexcept { return static_cast<std::tuple<T...> const&>(*this); }

    using std::tuple<T...>::tuple;
    using std::tuple<T...>::swap;

    template <typename... U,
              typename = std::enable_if_t<std::is_constructible_v<std::tuple<T...>, std::tuple<U...>&>>>
    constexpr tuple(std::tuple<U...>& other) : std::tuple<T...>{other} { }

    template <typename... U,
              typename = std::enable_if_t<std::is_constructible_v<std::tuple<T...>, std::tuple<U...> const&>>>
    constexpr tuple(std::tuple<U...> const& other) : std::tuple<T...>{other} { }

    template <typename... U,
              typename = std::enable_if_t<std::is_constructible_v<std::tuple<T...>, std::tuple<U...>&&>>>
    constexpr tuple(std::tuple<U...>&& other) : std::tuple<T...>{std::move(other)} { }

    template <typename... U,
              typename = std::enable_if_t<std::is_constructible_v<std::tuple<T...>, std::tuple<U...> const&&>>>
    constexpr tuple(std::tuple<U...> const&& other) : std::tuple<T...>{std::move(other)} { }

    template <typename... U,
              typename = std::enable_if_t<std::is_assignable_v<decltype(std::declval<tuple<U...>&>().base()), std::tuple<T...> const&>>,
              typename = std::enable_if_t<sizeof...(T) == sizeof...(U)>>
    constexpr tuple& operator=(std::tuple<U...> const& other)
    {
        base() = other;
        return *this;
    }

    template <typename... U,
              typename = std::enable_if_t<std::is_assignable_v<decltype(std::declval<tuple<U...>&&>().base()), std::tuple<T...>&&>>,
              typename = std::enable_if_t<sizeof...(T) == sizeof...(U)>>
    constexpr tuple& operator=(std::tuple<U...>&& other)
    {
        base() = std::move(other);
        return *this;
    }

    template <typename... U,
              typename = std::enable_if_t<!std::is_assignable_v<decltype(std::declval<tuple<U...> const&>().base()), std::tuple<T...> const&>>,
              typename = std::enable_if_t<sizeof...(T) == sizeof...(U)>>
    constexpr tuple const& operator=(std::tuple<U...> const& other) const
    {
        tuple_transform([](auto& l, auto& r) { l = r; }, base(), other);
        return *this;
    }

    template <typename... U,
              typename = std::enable_if_t<!std::is_assignable_v<decltype(std::declval<tuple<U...> const&&>().base()), std::tuple<T...>&&>>,
              typename = std::enable_if_t<sizeof...(T) == sizeof...(U)>>
    constexpr tuple const& operator=(std::tuple<U...>&& other) const
    {
        tuple_transform([](auto& l, auto& r) { l = r; }, base(), other);
        return *this;
    }

    constexpr void swap(tuple const& other) const
      noexcept((std::is_nothrow_swappable_v<T const> && ...))
    {
        tuple_transform([](auto& l, auto& r) { using std::swap; swap(l, r); }, base(), other);
    }

    template <typename... U>
    operator std::tuple<U...>() const
    {
        return static_cast<std::tuple<T...> const&>(*this);
    }
};

template <typename... T>
constexpr void swap(tuple<T...> const& lhs, tuple<T...> const& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

#endif

} // namespace flat_map::detail

#ifndef __cpp_lib_ranges_zip
FLATMAP_BEGIN_STD

template <typename... T>
struct tuple_size<flat_map::detail::tuple<T...>>
  : public tuple_size<std::tuple<T...>> { };

template <std::size_t I, typename... T>
struct tuple_element<I, flat_map::detail::tuple<T...>>
  : public tuple_element<I, std::tuple<T...>> { };

FLATMAP_END_STD
#endif
