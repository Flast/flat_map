// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>

#include "flat_map/enum.hpp"

namespace flat_map::detail
{

#ifndef FLAT_MAP_USE_NAIVE_IUSM

template <typename T>
struct _buffer_span
{
    T* _ptr;
    std::size_t _len;

    auto begin() { return _ptr; }
    auto end() { return _ptr + _len; }

    auto subbuf(std::size_t n) { return _buffer_span{_ptr, std::min(n, _len)}; }
};

template <typename T, typename Allocator>
struct _temporary_buffer : _buffer_span<T>
{
    Allocator _alloc;

    _temporary_buffer(std::size_t n, Allocator alloc)
      : _buffer_span<T>{alloc.allocate(n), n}, _alloc{std::move(alloc)} { }

    _temporary_buffer(_temporary_buffer const&) = delete;
    _temporary_buffer(_temporary_buffer&&) = delete;

    ~_temporary_buffer() noexcept
    {
        _alloc.deallocate(this->_ptr, this->_len);
    }
};

// Both of [first1, last1) and [first2, last2) should satisfy `desire`.
template <typename ForwardIterator, typename Compare, typename T>
inline ForwardIterator
_inplace_unique_merge(ForwardIterator first1, ForwardIterator last1, ForwardIterator first2, ForwardIterator last2,
                      range_order desire, Compare const& comp, _buffer_span<T> buffer)
{
    buffer = buffer.subbuf(std::distance(first1, last1));
    std::uninitialized_move(first1, last1, buffer.begin());

    auto itr = buffer.begin();
    while (first2 != last2 && itr != buffer.end())
    {
        if (comp(*first2, *itr))
        {
            *first1++ = std::move(*first2++);
        }
        else
        {
            if (desire == range_order::unique_sorted && !comp(*itr, *first2)) { ++first2; }
            *first1++ = std::move(*itr++);
        }
    }
    first1 = std::move(itr, buffer.end(), first1);
    if (first1 != first2)
    {
        first1 = std::move(first2, last2, first1);
    }
    else
    {
        first1 = last2;
    }

    std::destroy(buffer.begin(), buffer.end());

    return first1;
}

template <typename RandomAccessIterator, typename Compare, typename T>
inline RandomAccessIterator
_stable_unique_sort(RandomAccessIterator first, RandomAccessIterator last,
                    range_order desire, Compare const& comp, _buffer_span<T> buffer)
{
    auto len = std::distance(first, last);
    switch (len)
    {
    case 0:
    case 1:
        return last;
    case 2:
        if (comp(*first, *std::next(first))) { return last; }
        if (comp(*std::next(first), *first))
        {
            using std::iter_swap;
            iter_swap(first, std::next(first));
            return last;
        }
        return desire == range_order::unique_sorted ? std::next(first) : last;
    }

    // TODO: use insertion sort for short range

    auto mid = std::next(first, len / 2);
    auto left = _stable_unique_sort(first, mid, desire, comp, buffer);
    auto right = _stable_unique_sort(mid, last, desire, comp, buffer);
    return _inplace_unique_merge(first, left, mid, right, desire, comp, buffer);
}

// Specialized algorithm for compbining std::inplace_merge, std::stable_sort, and std::unique.
//
// Pre-requisite
// - [first, middle) is already sorted and uniqued (if need), i.e. should satisfy `desire`
//
// Return value
//  Past the end iterator of merged range
template <typename RandomAccessIterator, typename Compare, typename Allocator>
inline RandomAccessIterator
inplace_unique_sort_merge(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                          range_order desire, range_order order, Compare comp, Allocator alloc)
{
    auto len1 = std::distance(first, middle);
    auto len2 = std::distance(middle, last);
    if (len1 == 0 || len2 == 0) { return last; }

    using value_type = typename std::iterator_traits<RandomAccessIterator>::value_type;
    auto buflen = std::max<std::size_t>(len1, (len2 + 1) / 2);
    _temporary_buffer<value_type, Allocator> buffer{buflen, std::move(alloc)};

    RandomAccessIterator itr;
    switch (order)
    {
    case range_order::no_ordered:
    case range_order::uniqued:
        last = _stable_unique_sort(middle, last, desire, std::ref(comp), buffer);
        if (true) ; else

    case range_order::sorted:
        if (desire == range_order::unique_sorted)
        {
            last = std::unique(middle, last, [&comp](auto& l, auto& r) { return !comp(l, r) && !comp(r, l); });
        }
        [[gnu::fallthrough]];

    case range_order::unique_sorted:
        itr = _inplace_unique_merge(first, middle, middle, last, desire, comp, buffer);
        break;
    }

    return itr;
}

#else // FLAT_MAP_USE_NAIVE_IUSM

template <typename RandomAccessIterator, typename Compare, typename Allocator>
inline RandomAccessIterator
inplace_unique_sort_merge(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                          range_order desire, range_order order, Compare comp, [[maybe_unused]] Allocator alloc)
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
    case range_order::no_ordered:
    case range_order::uniqued:
        __builtin_unreachable();

    case range_order::sorted:
        return last;

    case range_order::unique_sorted:
        return std::unique(first, last, [&comp](auto& l, auto& r) { return !comp(l, r) && !comp(r, l); });
    }
}

#endif // FLAT_MAP_USE_NAIVE_IUSM

} // namespace flat_map::detail
