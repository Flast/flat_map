// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <cassert>
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>

#include "flat_map/enum.hpp"

namespace flat_map::detail
{

inline constexpr bool is_sorted(range_order order)
{
    switch (order)
    {
    default:
    case range_order::no_ordered:
    case range_order::uniqued:
        return false;
    case range_order::sorted:
    case range_order::unique_sorted:
        return true;
    }
}

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

// Both of [first1, last1) and [first2, last2) should satisfy `Desire`.
template <range_order Desire, typename ForwardIterator, typename Compare, typename T>
inline ForwardIterator
_inplace_unique_merge(ForwardIterator first1, ForwardIterator last1, ForwardIterator first2, ForwardIterator last2,
                      Compare const& comp, _buffer_span<T> buffer)
{
    static_assert(is_sorted(Desire));

    while (first1 != last1 && comp(*first1, *first2)) { ++first1; }
    if (first1 == last1)
    {
        if (last1 != first2)
        {
            last2 = std::move(first2, last2, last1);
        }
        return last2;
    }

    buffer = buffer.subbuf(std::distance(first1, last1));
    std::uninitialized_move(first1, last1, buffer.begin());

    auto itr = buffer.begin();
    // Checking itr first allows to merge with std::move's codition branch.
    //  while (itr != buffer.end() && first2 != last2)
    //      ...
    //  while (itr != buffer.end()) // std::move(itr. buffer.end(), ...)
    //  {   // jump here, or
    //  }
    //  // here since `itr != buffer.end()` is already evaluated by leading while loop.
    while (itr != buffer.end() && first2 != last2)
    {
        if (comp(*first2, *itr))
        {
            *first1++ = std::move(*first2++);
        }
        else
        {
            if constexpr (Desire == range_order::unique_sorted) if (!comp(*itr, *first2)) { ++first2; }
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

template <typename Iterator>
inline constexpr std::ptrdiff_t _switch_stable_sort_implementation_v = 16;

template <range_order Desire, typename BidirectionalIterator, typename Compare>
inline BidirectionalIterator
_insertion_unique_sort(BidirectionalIterator first, BidirectionalIterator last, Compare const& comp)
{
    // This algorithm assumes that the range is longer than 2.
    assert(std::distance(first, last) > 2);

    // NOTE: [first, i) is sorted range, and [j, last) is not processed yet.
    auto j = std::next(first);
    auto i = j++;

    if (comp(*i, *first))
    {
        using std::iter_swap;
        iter_swap(first, i++);
    }
    else
    {
        if constexpr (Desire == range_order::sorted) { ++i; }
        else if (comp(*first, *i)) { ++i; }
    }

    for ( ; j != last; ++j)
    {
        auto k = std::find_if(std::make_reverse_iterator(i), std::make_reverse_iterator(first),
                              [&comp, &j](auto& v) { return !comp(*j, v); });
        if (k.base() != i)
        {
            if constexpr (Desire == range_order::unique_sorted)
            {
                if (k.base() != first && !comp(*k, *j)) { continue; } // skip duplication
                if (i != j)
                {
                    std::move_backward(k.base(), i++, i);
                    *k.base() = std::move(*j);
                    continue;
                }
            }
            auto tmp = std::move(*j);
            std::move_backward(k.base(), i++, i);
            *k.base() = std::move(tmp);
        }
        else
        {
            if constexpr (Desire == range_order::unique_sorted)
            {
                if (!comp(*k, *j)) { continue; } // skip duplication
                if (i != j)
                {
                    *i++ = std::move(*j);
                    continue;
                }
            }
            ++i;
        }
    }

    return i;
}

template <range_order Desire, typename BidirectionalIterator, typename Compare, typename T>
inline BidirectionalIterator
_stable_unique_sort(BidirectionalIterator first, BidirectionalIterator last, Compare const& comp, _buffer_span<T> buffer)
{
    static_assert(is_sorted(Desire));

    auto len = std::distance(first, last);
    switch (len)
    {
    case 0:
    case 1:
        return last;
    case 2:
        if (comp(*std::next(first), *first))
        {
            using std::iter_swap;
            iter_swap(first, std::next(first));
            return last;
        }
        if constexpr (Desire == range_order::unique_sorted)
        {
            if (!comp(*first, *std::next(first))) { return std::next(first); }
        }
        return last;
    }

    if (len <= _switch_stable_sort_implementation_v<BidirectionalIterator>)
    {
        return _insertion_unique_sort<Desire>(first, last, comp);
    }

    auto mid = std::next(first, len / 2);
    auto left = _stable_unique_sort<Desire>(first, mid, comp, buffer);
    auto right = _stable_unique_sort<Desire>(mid, last, comp, buffer);
    return _inplace_unique_merge<Desire>(first, left, mid, right, comp, buffer);
}

// Specialized algorithm for compbining std::inplace_merge, std::stable_sort, and std::unique.
//
// Pre-requisite
// - [first, middle) is already sorted and uniqued (if need), i.e. should satisfy `Desire`
//
// Return value
//  Past the end iterator of merged range
template <range_order Desire, typename RandomAccessIterator, typename Compare, typename Allocator>
inline RandomAccessIterator
_inplace_unique_sort_merge(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                           range_order order, Compare const& comp, Allocator alloc)
{
    static_assert(is_sorted(Desire));

    auto len1 = std::distance(first, middle);
    auto len2 = std::distance(middle, last);
    if (len2 == 0) { return last; }

    using value_type = typename std::iterator_traits<RandomAccessIterator>::value_type;
    auto buflen = std::max<std::size_t>(len1, (len2 + 1) / 2);
    _temporary_buffer<value_type, Allocator> buffer{buflen, std::move(alloc)};

    switch (order)
    {
    case range_order::no_ordered:
    case range_order::uniqued:
        last = _stable_unique_sort<Desire>(middle, last, comp, buffer);
        break;

    case range_order::sorted:
        if constexpr (Desire == range_order::unique_sorted)
        {
            last = std::unique(middle, last, [&comp](auto& l, auto& r) { return !comp(l, r) && !comp(r, l); });
        }
        break;

    case range_order::unique_sorted:
        break;
    }

    return _inplace_unique_merge<Desire>(first, middle, middle, last, comp, buffer);
}

template <range_order Desire, typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
_inplace_unique_sort_merge_naive(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                                 range_order order, Compare comp)
{
    static_assert(is_sorted(Desire));

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
    switch (Desire)
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

template <typename RandomAccessIterator, range_order Desire, typename Compare, typename Allocator>
inline RandomAccessIterator
inplace_unique_sort_merge(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                          range_order_t<Desire>, range_order order, Compare comp, [[maybe_unused]] Allocator alloc)
{
    static_assert(is_sorted(Desire));
#ifndef FLAT_MAP_USE_NAIVE_IUSM
    return _inplace_unique_sort_merge<Desire>(first, middle, last, order, comp, std::move(alloc));
#else // FLAT_MAP_USE_NAIVE_IUSM
    return _inplace_unique_sort_merge_naive<Desire>(first, middle, last, order, comp);
#endif // FLAT_MAP_USE_NAIVE_IUSM
}

} // namespace flat_map::detail
