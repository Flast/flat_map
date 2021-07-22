// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <functional>

#include "flat_map/enum.hpp"

namespace flat_map::detail
{

template <typename RandomAccessIterator, typename Compare, typename Allocator>
inline RandomAccessIterator
inplace_unique_sort_merge(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                          range_order desire, range_order order, Compare comp, [[maybe_unused]] Allocator const& alloc)
{
    switch (order)
    {
    case range_order::no_ordered:
    case range_order::uniqued:
        std::stable_sort(first, last, std::ref(comp));
        break;

    case range_order::sorted:
    case range_order::unique_sorted:
        std::inplace_merge(first, middle, last, std::ref(comp));
        break;
    }
    switch (desire)
    {
    default: __builtin_unreachable();
    case range_order::sorted:
        return last;

    case range_order::unique_sorted:
        return std::unique(first, last, [&comp](auto& l, auto& r) { return !comp(l, r) && !comp(r, l); });
    }
}

} // namespace flat_map::detail
