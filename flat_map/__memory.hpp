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

    explicit fake_allocator(AllocatorTuple&& tuple)      noexcept : AllocatorTuple{std::move(tuple)} { }
    explicit fake_allocator(AllocatorTuple const& tuple) noexcept : AllocatorTuple{tuple} { }

    template <std::size_t I> decltype(auto) get()       noexcept { return std::get<I>(static_cast<AllocatorTuple&>(*this)); }
    template <std::size_t I> decltype(auto) get() const noexcept { return std::get<I>(static_cast<AllocatorTuple&>(*this)); }

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
    return detail::fake_allocator{std::tuple<Allocators...>(std::forward<Allocators>(alloc)...)};
}

} // namespace flat_map
