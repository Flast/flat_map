// Copyright (c) 2021,2024 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <memory>
#include <new>
#include <tuple>

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

    fake_allocator() noexcept = default;
    fake_allocator(fake_allocator const&) = default;
    fake_allocator(fake_allocator&&) = default;

    explicit fake_allocator(AllocatorTuple const& tuple) noexcept : AllocatorTuple{tuple} { }
    explicit fake_allocator(AllocatorTuple&& tuple)      noexcept : AllocatorTuple{std::move(tuple)} { }

    template <typename... Allocators>
    explicit fake_allocator(std::allocator_arg_t, Allocators&&... allocs) noexcept : AllocatorTuple{std::forward<Allocators>(allocs)...} { }

    fake_allocator& operator=(fake_allocator const&) noexcept = default;
    fake_allocator& operator=(fake_allocator&&) noexcept = default;

    template <std::size_t I> decltype(auto) get()       noexcept { return std::get<I>(static_cast<AllocatorTuple&>(*this)); }
    template <std::size_t I> decltype(auto) get() const noexcept { return std::get<I>(static_cast<AllocatorTuple const&>(*this)); }

    [[noreturn]]
    void* allocate(std::size_t) { throw std::bad_alloc(); }
    constexpr void deallocate(void*, std::size_t) { }

    constexpr bool operator==(fake_allocator) const { return true; }
    constexpr bool operator!=(fake_allocator) const { return false; }
};

} // namespace flat_map::detail

template <typename... Allocators>
auto forward_allocator(Allocators&&... alloc)
{
    return detail::fake_allocator<std::tuple<Allocators...>>{std::allocator_arg, std::forward<Allocators>(alloc)...};
}

} // namespace flat_map
