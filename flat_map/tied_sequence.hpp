// Copyright (c) 2021,2023-2024 Kohei Takahashi
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
class zip_iterator
{
    template <typename... Itr>
    friend class zip_iterator;

    template <typename... Sequences>
    friend class ::flat_map::tied_sequence;

    std::tuple<Iterators...> _it;

public:
    using difference_type = std::ptrdiff_t;
    using value_type = tuple<typename std::iterator_traits<Iterators>::value_type...>;
    using pointer = tuple<typename std::iterator_traits<Iterators>::pointer...>;
    using reference = tuple<typename std::iterator_traits<Iterators>::reference...>;
    using iterator_category = std::common_type_t<typename std::iterator_traits<Iterators>::iterator_category...>;

public:
    zip_iterator() = default;

    zip_iterator(std::tuple<Iterators...> itrs) : _it{itrs} { }

    template <typename... Itrs>
    zip_iterator(zip_iterator<Itrs...> other) : _it{std::move(other._it)} { }

    zip_iterator(zip_iterator const&) = default;
    zip_iterator(zip_iterator&&) = default;

    template <typename... Itrs>
    zip_iterator& operator=(zip_iterator<Itrs...> other)
    {
        _it = std::move(other._it);
        return *this;
    }

    zip_iterator& operator=(zip_iterator const&) = default;
    zip_iterator& operator=(zip_iterator&&) = default;

    template <std::size_t N>
    constexpr auto base() const { return std::get<N>(_it); }

private:
    template <std::size_t... N>
    constexpr reference _dereference(std::index_sequence<N...>) const { return std::tie(*std::get<N>(_it)...); }

public:
    constexpr reference operator*() const { return _dereference(std::index_sequence_for<Iterators...>{}); }

    // XXX: operator-> requires returning "raw pointer" or call op-> recursively until getting "raw pointer".
    // constexpr pointer operator->() const;

    constexpr zip_iterator& operator++()
    {
        tuple_reduction([](auto&... it) { (++it, ...); }, _it);
        return *this;
    }

    constexpr zip_iterator operator++(int)
    {
        auto copy = *this;
        operator++();
        return copy;
    }

    constexpr zip_iterator& operator--()
    {
        tuple_reduction([](auto&... it) { (--it, ...); }, _it);
        return *this;
    }

    constexpr zip_iterator operator--(int)
    {
        auto copy = *this;
        operator--();
        return copy;
    }

    constexpr zip_iterator& operator+=(difference_type n)
    {
        tuple_reduction([n](auto&... it) { ((it += n), ...); }, _it);
        return *this;
    }

    constexpr zip_iterator& operator-=(difference_type n)
    {
        tuple_reduction([n](auto&... it) { ((it -= n), ...); }, _it);
        return *this;
    }

    constexpr decltype(auto) operator[](difference_type n) const { return *(*this + n); }

    constexpr difference_type operator-(zip_iterator const& other) const { return std::get<0>(_it) - std::get<0>(other._it); }

    friend constexpr void swap(zip_iterator& lhs, zip_iterator& rhs) noexcept(noexcept(std::swap(lhs, rhs))) { std::swap(lhs._it, rhs._it); }

    friend constexpr void iter_swap(zip_iterator const& lhs, zip_iterator const& rhs)
        noexcept((std::is_nothrow_swappable_v<typename std::iterator_traits<Iterators>> && ...))
    {
        tuple_transform([](auto& l, auto& r) { using std::swap; swap(*l, *r); }, lhs._it, rhs._it);
    }

    template <typename... LIterators, typename... RIterators>
    friend constexpr bool operator==(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs);

    template <typename... LIterators, typename... RIterators>
    friend constexpr bool operator<(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs);
};

template <typename... Iterators>
constexpr auto operator+(zip_iterator<Iterators...> lhs, typename zip_iterator<Iterators...>::difference_type n)
{
    return lhs += n;
}

template <typename... Iterators>
constexpr auto operator+(typename zip_iterator<Iterators...>::difference_type n, zip_iterator<Iterators...> rhs)
{
    return rhs += n;
}

template <typename... Iterators>
constexpr auto operator-(zip_iterator<Iterators...> lhs, typename zip_iterator<Iterators...>::difference_type n)
{
    return lhs -= n;
}

template <typename... LIterators, typename... RIterators>
constexpr bool operator==(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs)
{
    return lhs._it == rhs._it;
}

template <typename... LIterators, typename... RIterators>
constexpr bool operator!=(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs)
{
    return !(lhs == rhs);
}

template <typename... LIterators, typename... RIterators>
constexpr bool operator<(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs)
{
    return lhs._it < rhs._it;
}

template <typename... LIterators, typename... RIterators>
constexpr bool operator>(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs)
{
    return rhs < lhs;
}

template <typename... LIterators, typename... RIterators>
constexpr bool operator<=(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs)
{
    return !(lhs > rhs);
}

template <typename... LIterators, typename... RIterators>
constexpr bool operator>=(zip_iterator<LIterators...> const& lhs, zip_iterator<RIterators...> const& rhs)
{
    return !(lhs < rhs);
}

template <std::size_t N, typename Iterator>
class unzip_iterator
{
    Iterator _base;

public:
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using value_type = std::tuple_element_t<N, typename std::iterator_traits<Iterator>::value_type>;
    using pointer = detail::copy_cv_t<value_type, typename std::iterator_traits<Iterator>::pointer>*;
    // using reference = detail::copy_cv_t<value_type, std::remove_reference_t<typename std::iterator_traits<Iterator>::reference>>&;
    using reference = decltype(std::get<N>(*std::declval<Iterator>()));
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

public:
    constexpr unzip_iterator() = default;
    explicit constexpr unzip_iterator(Iterator itr) : _base(itr) { }

    constexpr reference operator*() const { return std::forward<reference>(std::get<N>(*_base)); }
    constexpr pointer operator->() const { return &std::get<N>(*_base); }

    constexpr unzip_iterator& operator++() { ++_base; return *this; }
    constexpr unzip_iterator operator++(int) { return unzip_iterator{_base++}; }

    constexpr unzip_iterator& operator--() { --_base; return *this; }
    constexpr unzip_iterator operator--(int) { return unzip_iterator{_base--}; }

    constexpr unzip_iterator& operator+=(difference_type n) { _base += n; return *this; }
    constexpr unzip_iterator& operator-=(difference_type n) { _base -= n; return *this; }

    constexpr decltype(auto) operator[](difference_type n) const { return *(*this + n); }

    constexpr difference_type operator-(unzip_iterator const& other) const { return _base - other._base; }

    friend constexpr void swap(unzip_iterator& lhs, unzip_iterator& rhs) noexcept(std::is_nothrow_swappable_v<Iterator>)
    {
        using std::swap;
        return swap(lhs._base, rhs._base);
    }

    friend constexpr bool operator==(unzip_iterator const& lhs, unzip_iterator const& rhs) { return lhs._base == rhs._base; }
    friend constexpr bool operator<(unzip_iterator const& lhs, unzip_iterator const& rhs) { return lhs._base < rhs._base; }
};

template <std::size_t N, typename Iterator>
constexpr auto unzip(Iterator itr) { return unzip_iterator<N, Iterator>{itr}; }

template <std::size_t N, typename Iterator>
constexpr auto operator+(unzip_iterator<N, Iterator> lhs, typename unzip_iterator<N, Iterator>::difference_type n)
{
    return lhs += n;
}

template <std::size_t N, typename Iterator>
constexpr auto operator+(typename unzip_iterator<N, Iterator>::difference_type n, unzip_iterator<N, Iterator> rhs)
{
    return rhs += n;
}

template <std::size_t N, typename Iterator>
constexpr auto operator-(unzip_iterator<N, Iterator> lhs, typename unzip_iterator<N, Iterator>::difference_type n)
{
    return lhs -= n;
}

template <std::size_t N, typename Iterator>
constexpr bool operator!=(unzip_iterator<N, Iterator> const& lhs, unzip_iterator<N, Iterator> const& rhs)
{
    return !(lhs == rhs);
}

template <std::size_t N, typename Iterator>
constexpr bool operator>(unzip_iterator<N, Iterator> const& lhs, unzip_iterator<N, Iterator> const& rhs)
{
    return rhs < lhs;
}

template <std::size_t N, typename Iterator>
constexpr bool operator<=(unzip_iterator<N, Iterator> const& lhs, unzip_iterator<N, Iterator> const& rhs)
{
    return !(lhs > rhs);
}

template <std::size_t N, typename Iterator>
constexpr bool operator>=(unzip_iterator<N, Iterator> const& lhs, unzip_iterator<N, Iterator> const& rhs)
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
    using allocator_type = detail::fake_allocator<std::tuple<typename Sequences::allocator_type...>>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = std::tuple<typename Sequences::reference...>;
    using const_reference = std::tuple<typename Sequences::const_reference...>;
    using pointer = std::tuple<typename Sequences::pointer...>;
    using const_pointer = std::tuple<typename Sequences::const_pointer...>;
    using iterator = detail::zip_iterator<typename Sequences::iterator...>;
    using const_iterator = detail::zip_iterator<typename Sequences::const_iterator...>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    template <std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, allocator_type const& alloc)
      : _seq{alloc.template get<N>()...} { }

    template <std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, size_type count, allocator_type const& alloc)
      : _seq{Sequences(count, alloc.template get<N>())...} { }

    template <std::size_t... N, typename T>
    constexpr tied_sequence(std::index_sequence<N...>, size_type count, T&& value, allocator_type const& alloc)
      : _seq{Sequences(count, std::get<N>(std::forward<T>(value)), alloc.template get<N>())...} { }

    template <std::size_t... N, typename T>
    constexpr tied_sequence(std::index_sequence<N...>, T&& value, allocator_type const& alloc)
      : _seq{Sequences(std::get<N>(std::forward<T>(value)), alloc.template get<N>())...} { }

    template <std::size_t... N, typename T>
    constexpr tied_sequence(std::index_sequence<N...>, size_type count, T&& value, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(count, std::get<N>(std::forward<T>(value)), alloc)...} { }

    template <std::size_t... N, typename T>
    constexpr tied_sequence(std::index_sequence<N...>, T&& value, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(std::get<N>(std::forward<T>(value)), alloc)...} { }

    template <typename InputIterator, std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...> indices, InputIterator first, InputIterator last, allocator_type const& alloc)
      : tied_sequence{indices, typename std::iterator_traits<InputIterator>::iterator_category{}, first, last, alloc.template get<N>()...} { }

    template <typename InputIterator, std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, std::input_iterator_tag, InputIterator, InputIterator, typename Sequences::allocator_type const&...)
    {
        // TODO
        static_assert(sizeof(InputIterator) && false, "not implemented yet");
    }

    template <typename ForwardIterator, std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, std::forward_iterator_tag, ForwardIterator first, ForwardIterator last, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(detail::unzip<N>(first), detail::unzip<N>(last), alloc)...} { }

    template <std::size_t... N>
    constexpr tied_sequence(std::index_sequence<N...>, std::initializer_list<value_type> init, allocator_type const& alloc)
      : tied_sequence{init, alloc.template get<N>()...} { }

public:
    constexpr tied_sequence() noexcept((std::is_nothrow_default_constructible_v<Sequences> && ...))
#if FLAT_MAP_WORKAROUND(FLAT_MAP_COMPILER_GCC, < FLAT_MAP_COMPILER_VERSION(10,0,0))
    {}
#else
      = default;
#endif

    constexpr explicit tied_sequence(allocator_type const& alloc) noexcept
      : tied_sequence(std::index_sequence_for<Sequences...>{}, alloc) { }

    constexpr explicit tied_sequence(typename Sequences::allocator_type const&... alloc) noexcept : _seq{alloc...} { }

    constexpr tied_sequence(size_type count, value_type const& value)
      : tied_sequence(count, value, typename Sequences::allocator_type{}...) { }

    constexpr tied_sequence(size_type count, value_type const& value, allocator_type const& alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, count, value, alloc) { }

    constexpr tied_sequence(size_type count, value_type const& value, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, count, value, alloc...) { }

    constexpr tied_sequence(size_type count) : tied_sequence(count, typename Sequences::allocator_type{}...) { }

    constexpr tied_sequence(size_type count, allocator_type const& alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, count, alloc) { }

    constexpr tied_sequence(size_type count, typename Sequences::allocator_type const&... alloc)
      : _seq{Sequences(count, alloc)...} { }

    template <typename InputIterator>
    constexpr tied_sequence(InputIterator first, InputIterator last)
      : tied_sequence(first, last, typename Sequences::allocator_type{}...) { }

    template <typename InputIterator>
    constexpr tied_sequence(InputIterator first, InputIterator last, allocator_type const& alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, first, last, alloc) { }

    template <typename InputIterator>
    constexpr tied_sequence(InputIterator first, InputIterator last, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, typename std::iterator_traits<InputIterator>::iterator_category{}, first, last, alloc...) { }

    constexpr tied_sequence(tied_sequence const&) = default;

    constexpr tied_sequence(tied_sequence const& other, allocator_type const& alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, other._seq, alloc) { }

    constexpr tied_sequence(tied_sequence const& other, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, other._seq, alloc...) { }

    constexpr tied_sequence(tied_sequence&&) noexcept = default;

    constexpr tied_sequence(tied_sequence&& other, allocator_type const& alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, std::move(other._seq), alloc) { }

    constexpr tied_sequence(tied_sequence&& other, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, std::move(other._seq), alloc...) { }

    constexpr tied_sequence(std::initializer_list<value_type> init)
      : tied_sequence(init, typename Sequences::allocator_type{}...) { }

    constexpr tied_sequence(std::initializer_list<value_type> init, allocator_type const& alloc)
      : tied_sequence(std::index_sequence_for<Sequences...>{}, init, alloc) { }

    constexpr tied_sequence(std::initializer_list<value_type> init, typename Sequences::allocator_type const&... alloc)
      : tied_sequence(init.begin(), init.end(), alloc...) { }

    constexpr tied_sequence& operator=(tied_sequence const& other) = default;
    constexpr tied_sequence& operator=(tied_sequence&& other)
      noexcept(((std::allocator_traits<typename Sequences::allocator_type>::propagate_on_container_move_assignment::value
              || std::allocator_traits<typename Sequences::allocator_type>::is_always_equal::value) && ...)) = default;

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
        static_assert(sizeof(InputIterator) && false, "not implemented yet");
    }

    template <typename ForwardIterator, std::size_t... N>
    constexpr void _assign(ForwardIterator first, ForwardIterator last, std::index_sequence<N...>, std::forward_iterator_tag)
    {
        detail::tuple_transform(
            [](auto& c, auto first, auto last) { c.assign(first, last); },
            _seq,
            std::tuple{detail::unzip<N>(first)...},
            std::tuple{detail::unzip<N>(last)...}
        );
    }

public:
    template <typename InputIterator>
    constexpr void assign(InputIterator first, InputIterator last)
    {
        _assign(first, last, std::index_sequence_for<Sequences...>{}, typename std::iterator_traits<InputIterator>::iterator_category{});
    }

    constexpr void assign(std::initializer_list<value_type> ilist) { *this = ilist; }

    constexpr allocator_type get_allocator() const noexcept
    {
        return std::apply([](auto&... seqs) { return allocator_type{std::allocator_arg, seqs.get_allocator()...}; }, _seq);
    }

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

    constexpr pointer data() noexcept { return detail::tuple_transform([](auto& c) { return c.data(); }, _seq); }

    constexpr const_pointer data() const noexcept(noexcept(std::declval<tied_sequence*>()->data())) { return const_cast<tied_sequence*>(this)->data(); }

    constexpr iterator begin() noexcept { return detail::tuple_transform([](auto& c) { return c.begin(); }, _seq); }
    constexpr const_iterator begin() const noexcept { return detail::tuple_transform([](auto& c) { return c.begin(); }, _seq); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr iterator end() noexcept { return detail::tuple_transform([](auto& c) { return c.end(); }, _seq); }
    constexpr const_iterator end() const noexcept { return detail::tuple_transform([](auto& c) { return c.end(); }, _seq); }
    constexpr const_iterator cend() const noexcept { return end(); }
    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
    constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    [[nodiscard]] constexpr bool empty() const noexcept { return std::get<0>(_seq).empty(); }
    constexpr size_t size() const noexcept { return std::get<0>(_seq).size(); }

public:
    constexpr size_t max_size() const noexcept { return detail::tuple_reduction([](auto&&... c) { return std::min({c.max_size()...}); }, _seq); }

    constexpr void reserve(size_type new_cap) { detail::tuple_reduction([new_cap](auto&... c) { (c.reserve(new_cap), ...); }, _seq); }

    constexpr size_type capacity() const noexcept { return detail::tuple_reduction([](auto&... c) { return std::min({c.capacity()...}); }, _seq); }

    constexpr void shrink_to_fit() { detail::tuple_reduction([](auto&... c) { (c.shrink_to_fit(), ...); }, _seq); }

    constexpr void clear() noexcept { detail::tuple_reduction([](auto&... c) { (c.clear(), ...); }, _seq); }

    constexpr iterator insert(const_iterator pos, value_type const& value)
    {
        // FIXME: exception neutral impl
        return detail::tuple_transform([](auto& c, auto pos, auto& value) { return c.insert(pos, value); }, _seq, pos._it, value);
    }

    constexpr iterator insert(const_iterator pos, value_type&& value)
    {
        // FIXME: exception neutral impl
        return detail::tuple_transform([](auto& c, auto pos, auto&& value) { return c.insert(pos, std::move(value)); }, _seq, pos._it, std::move(value));
    }

    constexpr iterator insert(const_iterator pos, size_type count, value_type const& value)
    {
        // FIXME: exception neutral impl
        return detail::tuple_transform([count](auto& c, auto pos, auto& value) { return c.insert(pos, count, value); }, _seq, pos._it, value);
    }

private:
    template <typename InputIterator, std::size_t... N>
    constexpr iterator _insert(const_iterator, InputIterator, InputIterator, std::index_sequence<N...>, std::input_iterator_tag)
    {
        // TODO
        static_assert(sizeof(InputIterator) && false, "not implemented yet");
    }

    template <typename ForwardIterator, std::size_t... N>
    constexpr iterator _insert(const_iterator pos, ForwardIterator first, ForwardIterator last, std::index_sequence<N...>, std::forward_iterator_tag)
    {
        // FIXME: exception neutral impl
        return detail::tuple_transform(
            [](auto& c, auto pos, auto first, auto last) { return c.insert(pos, first, last); },
            _seq,
            pos._it,
            std::tuple{detail::unzip<N>(first)...},
            std::tuple{detail::unzip<N>(last)...}
        );
    }

    template <typename Container, typename Iterator, std::size_t... N, typename... Args>
    constexpr auto _emplace_helper(Container& c, Iterator pos, std::index_sequence<N...>, std::tuple<Args...> const& args)
    {
        return c.emplace(pos, std::forward<Args>(std::get<N>(args))...);
    }

    template <typename Container, typename Iterator, std::size_t... N, typename... Args>
    constexpr auto _emplace_helper(Container& c, Iterator pos, std::index_sequence<N...>, std::tuple<Args...>&& args)
    {
        return c.emplace(pos, std::forward<Args>(std::get<N>(args))...);
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

    template <typename... Args>
    constexpr iterator emplace(const_iterator pos, std::piecewise_construct_t, Args&&... args)
    {
        // FIXME: exception neutral impl
        auto inserter = [this](auto& c, auto pos, auto&& args)
        {
            using indices_t = std::make_index_sequence<std::tuple_size_v<detail::remove_cvref_t<decltype(args)>>>;
            return this->_emplace_helper(c, pos, indices_t{}, std::forward<decltype(args)>(args));
        };
        return detail::tuple_transform(std::move(inserter), _seq, pos._it, std::forward_as_tuple(std::forward<Args>(args)...));
    }

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

    constexpr void pop_back() { detail::tuple_reduction([](auto&... c) { (c.pop_back(), ...); }, _seq); }

    constexpr void resize(size_type count) { detail::tuple_reduction([count](auto&... c) { (c.resize(count), ...); }, _seq); }

    constexpr void resize(size_type count, value_type const& value)
    {
        detail::tuple_transform([count](auto& c, auto& value) { c.resize(count, value); }, _seq, value);
    }

    constexpr void swap(tied_sequence& other)
      noexcept(((std::allocator_traits<typename Sequences::allocator_type>::propagate_on_container_swap::value
              || std::allocator_traits<typename Sequences::allocator_type>::is_always_equal::value) && ...))
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
