// Copyright (c) 2021,2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "flat_map/__config.hpp"
#include "flat_map/__memory.hpp"
#include "flat_map/__tuple.hpp"
#include "flat_map/__type_traits.hpp"

namespace flat_map
{

namespace detail
{

template <typename... Iterators>
class tied_sequence_iterator
{
    template <typename... Itr>
    friend class tied_sequence_iterator;

    template <typename... Sequences>
    friend class ::flat_map::tied_sequence;

    std::tuple<Iterators...> _it;

public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<typename std::iterator_traits<Iterators>::value_type...>;
    using pointer = std::tuple<typename std::iterator_traits<Iterators>::pointer...>;
    using reference = std::tuple<typename std::iterator_traits<Iterators>::reference...>;
    using iterator_category = std::common_type_t<typename std::iterator_traits<Iterators>::iterator_category...>;

public:
    tied_sequence_iterator() = default;

    tied_sequence_iterator(std::tuple<Iterators...> itrs) : _it{itrs} { }

    template <typename... Itrs>
    tied_sequence_iterator(tied_sequence_iterator<Itrs...> other) : _it{std::move(other._it)} { }

    tied_sequence_iterator(tied_sequence_iterator const&) = default;
    tied_sequence_iterator(tied_sequence_iterator&&) = default;

    template <typename... Itrs>
    tied_sequence_iterator& operator=(tied_sequence_iterator<Itrs...> other)
    {
        _it = std::move(other._it);
        return *this;
    }

    tied_sequence_iterator& operator=(tied_sequence_iterator const&) = default;
    tied_sequence_iterator& operator=(tied_sequence_iterator&&) = default;

    template <std::size_t N>
    constexpr auto base() const { return std::get<N>(_it); }

    constexpr reference operator*() const { return tuple_transform([](auto... it) { return std::tie(*it...); }, _it); }

    // XXX: operator-> requires returning "raw pointer" or call op-> recursively until getting "raw pointer".
    // constexpr pointer operator->() const;

    constexpr tied_sequence_iterator& operator++()
    {
        tuple_transform([](auto&... it) { (++it, ...); }, _it);
        return *this;
    }

    constexpr tied_sequence_iterator operator++(int)
    {
        auto copy = *this;
        operator++();
        return copy;
    }

    constexpr tied_sequence_iterator& operator--()
    {
        tuple_transform([](auto&... it) { (--it, ...); }, _it);
        return *this;
    }

    constexpr tied_sequence_iterator operator--(int)
    {
        auto copy = *this;
        operator--();
        return copy;
    }

    constexpr tied_sequence_iterator& operator+=(difference_type n)
    {
        tuple_transform([n](auto&... it) { ((it += n), ...); }, _it);
        return *this;
    }

    constexpr tied_sequence_iterator& operator-=(difference_type n)
    {
        tuple_transform([n](auto&... it) { ((it -= n), ...); }, _it);
        return *this;
    }

    constexpr decltype(auto) operator[](difference_type n) const { return *(*this + n); }

    constexpr difference_type operator-(tied_sequence_iterator const& other) const { return std::get<0>(_it) - std::get<0>(other._it); }

    friend constexpr void swap(tied_sequence_iterator& lhs, tied_sequence_iterator& rhs) noexcept(noexcept(std::swap(lhs, rhs))) { std::swap(lhs._it, rhs._it); }

    friend constexpr void iter_swap(tied_sequence_iterator const& lhs, tied_sequence_iterator const& rhs)
        noexcept((std::is_nothrow_swappable_v<typename std::iterator_traits<Iterators>> && ...))
    {
        tuple_transform([](auto& l, auto& r) { using std::swap; swap(*l, *r); }, lhs._it, rhs._it);
    }

    friend constexpr bool operator==(tied_sequence_iterator const& lhs, tied_sequence_iterator const& rhs) { return lhs._it == rhs._it; }

    friend constexpr bool operator<(tied_sequence_iterator const& lhs, tied_sequence_iterator const& rhs) { return lhs._it < rhs._it; }
};

template <typename... Iterators>
constexpr auto operator+(tied_sequence_iterator<Iterators...> lhs, typename tied_sequence_iterator<Iterators...>::difference_type n)
{
    return lhs += n;
}

template <typename... Iterators>
constexpr auto operator+(typename tied_sequence_iterator<Iterators...>::difference_type n, tied_sequence_iterator<Iterators...> rhs)
{
    return rhs += n;
}

template <typename... Iterators>
constexpr auto operator-(tied_sequence_iterator<Iterators...> lhs, typename tied_sequence_iterator<Iterators...>::difference_type n)
{
    return lhs -= n;
}

template <typename... Iterators>
constexpr bool operator!=(tied_sequence_iterator<Iterators...> const& lhs, tied_sequence_iterator<Iterators...> const& rhs)
{
    return !(lhs == rhs);
}

template <typename... Iterators>
constexpr bool operator>(tied_sequence_iterator<Iterators...> const& lhs, tied_sequence_iterator<Iterators...> const& rhs)
{
    return rhs < lhs;
}

template <typename... Iterators>
constexpr bool operator<=(tied_sequence_iterator<Iterators...> const& lhs, tied_sequence_iterator<Iterators...> const& rhs)
{
    return !(lhs > rhs);
}

template <typename... Iterators>
constexpr bool operator>=(tied_sequence_iterator<Iterators...> const& lhs, tied_sequence_iterator<Iterators...> const& rhs)
{
    return !(lhs < rhs);
}

template <std::size_t N, typename Iterator>
class extracting_iterator
{
    Iterator _base;

public:
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using value_type = std::tuple_element_t<N, typename std::iterator_traits<Iterator>::value_type>;
    using pointer = detail::copy_cv_t<value_type, typename std::iterator_traits<Iterator>::pointer>*;
    using reference = detail::copy_cv_t<value_type, std::remove_reference_t<typename std::iterator_traits<Iterator>::reference>>&;
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

public:
    constexpr extracting_iterator() = default;
    explicit constexpr extracting_iterator(Iterator itr) : _base(itr) { }

    constexpr reference operator*() const { return std::get<N>(*_base); }
    constexpr pointer operator->() const { return &std::get<N>(*_base); }

    constexpr extracting_iterator& operator++() { ++_base; return *this; }
    constexpr extracting_iterator operator++(int) { return _base++; }

    constexpr extracting_iterator& operator--() { --_base; return *this; }
    constexpr extracting_iterator operator--(int) { return _base--; }

    constexpr extracting_iterator& operator+=(difference_type n) { _base += n; return *this; }
    constexpr extracting_iterator& operator-=(difference_type n) { _base -= n; return *this; }

    constexpr decltype(auto) operator[](difference_type n) const { return *(*this + n); }

    constexpr difference_type operator-(extracting_iterator const& other) const { return _base - other._base; }

    friend constexpr void swap(extracting_iterator& lhs, extracting_iterator& rhs) noexcept(std::is_nothrow_swappable_v<Iterator>)
    {
        using std::swap;
        return swap(lhs._base, rhs._base);
    }

    friend constexpr bool operator==(extracting_iterator const& lhs, extracting_iterator const& rhs) { return lhs._base == rhs._base; }
    friend constexpr bool operator<(extracting_iterator const& lhs, extracting_iterator const& rhs) { return lhs._base < rhs._base(); }
};

template <std::size_t N, typename Iterator>
constexpr auto extractor(Iterator itr) { return extracting_iterator<N, Iterator>{itr}; }

template <std::size_t N, typename Iterator>
constexpr auto operator+(extracting_iterator<N, Iterator> lhs, typename extracting_iterator<N, Iterator>::difference_type n)
{
    return lhs += n;
}

template <std::size_t N, typename Iterator>
constexpr auto operator+(typename extracting_iterator<N, Iterator>::difference_type n, extracting_iterator<N, Iterator> rhs)
{
    return rhs += n;
}

template <std::size_t N, typename Iterator>
constexpr auto operator-(extracting_iterator<N, Iterator> lhs, typename extracting_iterator<N, Iterator>::difference_type n)
{
    return lhs -= n;
}

template <std::size_t N, typename Iterator>
constexpr bool operator!=(extracting_iterator<N, Iterator> const& lhs, extracting_iterator<N, Iterator> const& rhs)
{
    return !(lhs == rhs);
}

template <std::size_t N, typename Iterator>
constexpr bool operator>(extracting_iterator<N, Iterator> const& lhs, extracting_iterator<N, Iterator> const& rhs)
{
    return rhs < lhs;
}

template <std::size_t N, typename Iterator>
constexpr bool operator<=(extracting_iterator<N, Iterator> const& lhs, extracting_iterator<N, Iterator> const& rhs)
{
    return !(lhs > rhs);
}

template <std::size_t N, typename Iterator>
constexpr bool operator>=(extracting_iterator<N, Iterator> const& lhs, extracting_iterator<N, Iterator> const& rhs)
{
    return !(lhs < rhs);
}

} // namespace flat_map::detail

template <typename... Sequences>
class tied_sequence
{
    std::tuple<Sequences...> _seq;

public:
    using value_type = std::tuple<typename Sequences::value_type...>;
    using allocator_type = detail::fake_allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = std::tuple<typename Sequences::reference...>;
    using const_reference = std::tuple<typename Sequences::const_reference...>;
    using pointer = std::tuple<typename Sequences::pointer...>;
    using const_pointer = std::tuple<typename Sequences::const_pointer...>;
    using iterator = detail::tied_sequence_iterator<typename Sequences::iterator...>;
    using const_iterator = detail::tied_sequence_iterator<typename Sequences::const_iterator...>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    template <std::size_t... N, typename T>
    constexpr tied_sequence(std::index_sequence<N...>, size_type count, T&& value, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(count, std::get<N>(std::forward<T>(value)), alloc)...} { }

    template <std::size_t... N, typename T>
    constexpr tied_sequence(std::index_sequence<N...>, T&& value, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(std::get<N>(std::forward<T>(value)), alloc)...} { }

    template <typename InputIterator, std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, std::input_iterator_tag, InputIterator, InputIterator, typename Sequences::allocator_type const&...)
    {
        // TODO
        static_assert(sizeof(InputIterator) && false, "not implemented yet");
    }

    template <typename ForwardIterator, std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, std::forward_iterator_tag, ForwardIterator first, ForwardIterator last, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(detail::extractor<N>(first), detail::extractor<N>(last), alloc)...} { }

public:
    constexpr tied_sequence() noexcept((std::is_nothrow_default_constructible_v<Sequences> && ... && true))
#if FLAT_MAP_WORKAROUND(FLAT_MAP_COMPILER_GCC, < FLAT_MAP_COMPILER_VERSION(10,0,0))
    {}
#else
      = default;
#endif

    constexpr explicit tied_sequence(allocator_type const&) noexcept(std::is_nothrow_default_constructible_v<tied_sequence>) : tied_sequence() { }

    constexpr explicit tied_sequence(typename Sequences::allocator_type const&... alloc) noexcept : _seq{alloc...} { }

    constexpr tied_sequence(size_type count, value_type const& value)
      : tied_sequence(count, value, typename Sequences::allocator_type{}...) { }

    constexpr tied_sequence(size_type count, value_type const& value, allocator_type const&)
      : tied_sequence(count, value) { }

    constexpr tied_sequence(size_type count, value_type const& value, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, count, value, alloc...) { }

    constexpr tied_sequence(size_type count) : tied_sequence(count, typename Sequences::allocator_type{}...) { }

    constexpr tied_sequence(size_type count, allocator_type const&) : tied_sequence(count) { }

    constexpr tied_sequence(size_type count, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(count, alloc)...} { }

    template <typename InputIterator>
    constexpr tied_sequence(InputIterator first, InputIterator last)
      : tied_sequence(first, last, typename Sequences::allocator_type{}...) { }

    template <typename InputIterator>
    constexpr tied_sequence(InputIterator first, InputIterator last, allocator_type const&) : tied_sequence(first, last) { }

    template <typename InputIterator>
    constexpr tied_sequence(InputIterator first, InputIterator last, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, typename std::iterator_traits<InputIterator>::iterator_category{}, first, last, alloc...) { }

    constexpr tied_sequence(tied_sequence const&) = default;

    constexpr tied_sequence(tied_sequence const& other, allocator_type const&) : tied_sequence(other) { }

    constexpr tied_sequence(tied_sequence const& other, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, other, alloc...) { }

    constexpr tied_sequence(tied_sequence&&) noexcept = default;

    constexpr tied_sequence(tied_sequence&& other, allocator_type const&) : tied_sequence(std::move(other)) { }

    constexpr tied_sequence(tied_sequence&& other, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, std::move(other), alloc...) { }

    constexpr tied_sequence(std::initializer_list<value_type> init)
      : tied_sequence(init, typename Sequences::allocator_type{}...) { }

    constexpr tied_sequence(std::initializer_list<value_type> init, allocator_type const&) : tied_sequence(init) { }

    constexpr tied_sequence(std::initializer_list<value_type> init, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(init.begin(), init.end(), alloc...) { }

    constexpr tied_sequence& operator=(tied_sequence const& other) = default;
    constexpr tied_sequence& operator=(tied_sequence&& other)
      noexcept(((std::allocator_traits<typename Sequences::allocator_type>::propagate_on_container_move_assignment::value
              || std::allocator_traits<typename Sequences::allocator_type>::is_always_equal::value) && ... && true)) = default;

    constexpr tied_sequence& operator=(std::initializer_list<value_type> ilist)
    {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    constexpr void assign(size_type count, value_type const& value)
    {
        detail::tuple_transform([count](auto& c, auto& value) { c.assign(count, value); }, _seq, value);
    }

private:
    template <typename InputIterator, std::size_t... N>
    constexpr void _assign(InputIterator, InputIterator, std::index_sequence<N...>, std::input_iterator_tag)
    {
        // TODO
        static_assert(sizeof(InputIterator) && false, "not impleneted yet");
    }

    template <typename ForwardIterator, std::size_t... N>
    constexpr void _assign(ForwardIterator first, ForwardIterator last, std::index_sequence<N...>, std::forward_iterator_tag)
    {
        detail::tuple_transform([&](auto&... c) { (c.assign(detail::extractor<N>(first), detail::extractor<N>(last)), ...); }, _seq);
    }

public:
    template <typename InputIterator>
    constexpr void assign(InputIterator first, InputIterator last)
    {
        _assign(first, last, std::index_sequence_for<Sequences...>{}, typename std::iterator_traits<InputIterator>::iterator_category{});
    }

    constexpr void assign(std::initializer_list<value_type> ilist) { *this = ilist; }

    constexpr allocator_type get_allocator() const noexcept { return {}; }

    constexpr reference at(size_type pos)
    {
        if (!(pos < size())) { throw std::out_of_range{"tied_sequence::at"}; }
        return operator[](pos);
    }

    constexpr const_reference at(size_type pos) const { return const_cast<tied_sequence*>(this)->at(pos); }

    constexpr reference operator[](size_type pos) { return *std::next(begin(), pos); }

    constexpr const_reference operator[](size_type pos) const { return const_cast<tied_sequence*>(this)->operator[](pos); }

    constexpr reference front() { return *begin(); }
    constexpr const_reference front() const { return *begin(); }
    constexpr reference back() { return *std::prev(end()); }
    constexpr const_reference back() const { return *std::prev(end()); }

    constexpr pointer data() noexcept((static_cast<void>(sizeof(std::declval<Sequences>().data())), ..., true))
    {
        return detail::tuple_transform([](auto&... c) { return std::tuple{c.data()...}; }, _seq);
    }

    constexpr const_pointer data() const noexcept(noexcept(std::declval<tied_sequence*>()->data())) { return const_cast<tied_sequence*>(this)->data(); }

    constexpr iterator begin() noexcept { return detail::tuple_transform([](auto&... c) { return std::tuple{c.begin()...}; }, _seq); }
    constexpr const_iterator begin() const noexcept { return detail::tuple_transform([](auto&... c) { return std::tuple{c.begin()...}; }, _seq); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr iterator end() noexcept { return detail::tuple_transform([](auto&... c) { return std::tuple{c.end()...}; }, _seq); }
    constexpr const_iterator end() const noexcept { return detail::tuple_transform([](auto&... c) { return std::tuple{c.end()...}; }, _seq); }
    constexpr const_iterator cend() const noexcept { return end(); }
    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
    constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    [[nodiscard]] constexpr bool empty() const noexcept { return std::get<0>(_seq).empty(); }
    constexpr size_t size() const noexcept { return std::get<0>(_seq).size(); }

    constexpr size_t max_size() const noexcept
    {
        return detail::tuple_transform([](auto&... c) { return std::min({c.max_size()...}); }, _seq);
    }

#if 0 // TODO
    constexpr void reserve(size_type new_cap) { detail::tuple_transform([new_cap](auto&... c) { c.reserve(new_cap); }, _seq); }

    constexpr size_type capacity() const noexcept { return detail::tuple_transform([](auto&... c) { return std::min({c.capacity()...}); }, _seq); }

    constexpr void shrink_to_fit() noexcept { detail::tuple_transform([](auto&... c) { (c.shrink_to_fit(), ...); }, _seq); }
#endif

    constexpr void clear() noexcept { detail::tuple_transform([](auto&... c) { (c.clear(), ...); }, _seq); }

    constexpr iterator insert(const_iterator pos, value_type const& value)
    {
        return detail::tuple_transform([](auto& c, auto pos, auto& value) { return c.insert(pos, value); }, _seq, pos._it, value);
    }

    constexpr iterator insert(const_iterator pos, value_type&& value)
    {
        return detail::tuple_transform([](auto& c, auto pos, auto&& value) { return c.insert(pos, std::move(value)); }, _seq, pos._it, std::move(value));
    }

    constexpr iterator insert(const_iterator pos, size_type count, value_type const& value)
    {
        return detail::tuple_transform([count](auto& c, auto pos, auto& value) { return c.insert(pos, count, value); }, _seq, pos._it, value);
    }

private:
    template <typename InputIterator, std::size_t... N>
    constexpr iterator _insert(const_iterator, InputIterator, InputIterator, std::index_sequence<N...>, std::input_iterator_tag)
    {
        // TODO
        static_assert(sizeof(InputIterator) && false, "not impleneted yet");
    }

    template <typename ForwardIterator, std::size_t... N>
    constexpr iterator _insert(const_iterator pos, ForwardIterator first, ForwardIterator last, std::index_sequence<N...>, std::forward_iterator_tag)
    {
        return detail::tuple_transform([&](auto&... c) { return std::tuple{c.insert(std::get<N>(pos._it), detail::extractor<N>(first), detail::extractor<N>(last))...}; }, _seq);
    }

public:
    template <typename InputIterator>
    constexpr iterator insert(const_iterator pos, InputIterator first, InputIterator last)
    {
        return _insert(pos, first, last, std::index_sequence_for<Sequences...>{}, typename std::iterator_traits<InputIterator>::iterator_category{});
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) { return insert(pos, ilist.begin(), ilist.end()); }

    template <typename... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args) { return insert(pos, value_type(std::forward<Args>(args)...)); }

    constexpr iterator erase(const_iterator pos) { return detail::tuple_transform([](auto& c, auto pos) { return c.erase(pos); }, _seq, pos._it); }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        return detail::tuple_transform([](auto& c, auto first, auto last) { return c.erase(first, last); }, _seq, first._it, last._it);
    }

    constexpr void push_back(value_type const& value) { insert(end(), value); }

    constexpr void push_back(value_type&& value) { insert(end(), std::move(value)); }

    template <typename... Args>
    constexpr reference emplace_back(Args&&... args)
    {
        push_back(value_type(std::forward<Args>(args)...));
        return back();
    }

    constexpr void pop_back() { detail::tuple_transform([](auto&... c) { (c.pop_back(), ...); }, _seq); }

    constexpr void resize(size_type count) { detail::tuple_transform([count](auto&... c) { (c.resize(count), ...); }, _seq); }

    constexpr void resize(size_type count, value_type const& value)
    {
        detail::tuple_transform([count](auto& c, auto& value) { c.resize(count, value); }, _seq, value);
    }

    constexpr void swap(tied_sequence& other)
      noexcept(((std::allocator_traits<typename Sequences::allocator_type>::propagate_on_container_swap::value
              || std::allocator_traits<typename Sequences::allocator_type>::is_always_equal::value) && ... && true))
    {
        detail::tuple_transform([](auto& lhs, auto& rhs) { using std::swap; swap(lhs, rhs); }, _seq, other._seq);
    }
};

template <typename... Sequences>
constexpr bool operator==(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

#ifndef FLAT_MAP_HAS_THREE_WAY_COMPARISON
template <typename... Sequences>
bool operator!=(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return !(lhs == rhs);
}

template <typename... Sequences>
bool operator<(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename... Sequences>
bool operator<=(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return !(rhs < lhs);
}

template <typename... Sequences>
bool operator>(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return rhs < lhs;
}

template <typename... Sequences>
bool operator>=(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return !(lhs < rhs);
}
#else
template <typename... Sequences>
constexpr auto operator<=>(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs)
{
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
#endif

template <typename... Sequences>
constexpr void swap(tied_sequence<Sequences...> const& lhs, tied_sequence<Sequences...> const& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

template <typename... Sequences, typename U>
constexpr typename tied_sequence<Sequences...>::size_type erase(tied_sequence<Sequences...>& c, U const& value)
{
    auto itr = std::remove(c.begin(), c.end(), value);
    auto r = std::distance(itr, c.end());
    c.erase(itr, c.end());
    return r;
}

template <typename... Sequences, typename Pred>
constexpr typename tied_sequence<Sequences...>::size_type erase_if(tied_sequence<Sequences...>& c, Pred pred)
{
    auto itr = std::remove_if(c.begin(), c.end(), std::move(pred));
    auto r = std::distance(itr, c.end());
    c.erase(itr, c.end());
    return r;
}

} // namespace flat_map
