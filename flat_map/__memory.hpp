// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <memory>
#include <new>

namespace flat_map::detail
{

struct fake_allocator
{
    template <typename>
    struct rebind { using other = fake_allocator; };

    using value_type = void;

    [[noreturn]]
    void* allocate(std::size_t) { throw std::bad_alloc(); }
    constexpr void deallocate(void*, std::size_t) { }

    constexpr bool operator==(fake_allocator) const { return true; }
    constexpr bool operator!=(fake_allocator) const { return false; }
};

} // namespace flat_map::detail
