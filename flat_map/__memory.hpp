// Copyright (c) 2021,2024 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <memory>
#include <new>
#include <tuple>
#include <type_traits>

namespace flat_map
{

namespace detail
{

template <typename AllocatorTuple>
struct fake_allocator : private AllocatorTuple
{
    template <typename U>
    struct rebind { using other = fake_allocator<U>; };

    using value_type = void;

    fake_allocator() noexcept(std::is_nothrow_default_constructible_v<AllocatorTuple>) = default;

    fake_allocator(fake_allocator const&) noexcept(std::is_nothrow_copy_constructible_v<AllocatorTuple>) = default;
    fake_allocator(fake_allocator&&)      noexcept(std::is_nothrow_move_constructible_v<AllocatorTuple>) = default;

    explicit fake_allocator(AllocatorTuple const& tuple) noexcept(std::is_nothrow_copy_constructible_v<AllocatorTuple>) : AllocatorTuple{tuple} { }
    explicit fake_allocator(AllocatorTuple&& tuple)      noexcept(std::is_nothrow_move_constructible_v<AllocatorTuple>) : AllocatorTuple{std::move(tuple)} { }

    template <typename... Allocators>
    explicit fake_allocator(std::allocator_arg_t, Allocators&&... allocs) noexcept : AllocatorTuple{std::forward<Allocators>(allocs)...} { }

    fake_allocator& operator=(fake_allocator const&) noexcept(std::is_nothrow_copy_assignable_v<AllocatorTuple>) = default;
    fake_allocator& operator=(fake_allocator&&)      noexcept(std::is_nothrow_move_assignable_v<AllocatorTuple>) = default;

    template <std::size_t I> decltype(auto) get()       noexcept { return std::get<I>(static_cast<AllocatorTuple&>(*this)); }
    template <std::size_t I> decltype(auto) get() const noexcept { return std::get<I>(static_cast<AllocatorTuple const&>(*this)); }

    [[noreturn]]
    void* allocate(std::size_t) { throw std::bad_alloc(); }
    constexpr void deallocate(void*, std::size_t) { }

    constexpr bool operator==(fake_allocator const& other) const noexcept { return static_cast<AllocatorTuple const&>(*this) == static_cast<AllocatorTuple const&>(other); }
    constexpr bool operator!=(fake_allocator const& other) const noexcept { return !operator==(other); }
};

} // namespace flat_map::detail

template <typename... Allocators>
auto forward_allocator(Allocators&&... alloc) noexcept
{
    return detail::fake_allocator<std::tuple<Allocators...>>{std::allocator_arg, std::forward<Allocators>(alloc)...};
}

} // namespace flat_map
