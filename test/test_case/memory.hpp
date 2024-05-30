// Copyright (c) 2024 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

template <typename T>
struct stateful_allocator : public std::allocator<T>
{
    std::string state;

    // Hide std::allocator::rebind for C++17 mode.
    template <typename U>
    struct rebind { using other = stateful_allocator<U>; };

    stateful_allocator() : state{"unspecified state"} {}

    stateful_allocator(char const* state) : state{state} {}

    stateful_allocator(stateful_allocator const&) noexcept = default;
    stateful_allocator(stateful_allocator&&) noexcept = default;

    template <typename U>
    stateful_allocator(stateful_allocator<U> const& other) noexcept : state{other.state} { }

    template <typename U>
    stateful_allocator(stateful_allocator<U>&& other) noexcept : state{std::move(other.state)} { }
};
