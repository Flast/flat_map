// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

#include "flat_map/__fwd.hpp"
#include "flat_map/__config.hpp"
#include "flat_map/__type_traits.hpp"
#include "flat_map/__flat_tree.hpp"

namespace flat_map
{

template <typename Key,
          typename Compare = std::less<Key>,
          typename Container = std::vector<Key>>
class flat_set : private detail::_flat_tree_base<flat_set<Key, Compare, Container>, Key, Compare, Container>
{
    using _super = typename flat_set::_flat_tree_base;

    // To lookup private comparator
    friend _super;

    static constexpr bool _is_uniq = true;

public:
    using key_type = typename _super::key_type;
    using value_type = typename _super::value_type;
    using size_type = typename _super::size_type;
    using difference_type = typename _super::difference_type;
    using key_compare = typename _super::key_compare;
    using value_compare = key_compare;
    using allocator_type = typename _super::allocator_type;
    using reference = typename _super::reference;
    using const_reference = typename _super::const_reference;
    using pointer = typename _super::pointer;
    using const_pointer = typename _super::const_pointer;
    using iterator = typename _super::iterator;
    using const_iterator = typename _super::const_iterator;
    using reverse_iterator = typename _super::reverse_iterator;
    using const_reverse_iterator = typename _super::const_reverse_iterator;
    using node_type = typename _super::node_type;
    using insert_return_type = typename _super::insert_return_type;

private:
    using _comparator = value_compare;

    static auto& _key_extractor(value_type const& value) { return value; }

public:
    flat_set() = default;

    explicit flat_set(Compare const& comp, allocator_type const& alloc = allocator_type())
      : _super{comp, alloc} { }

    explicit flat_set(allocator_type const& alloc)
      : _super{alloc} { }

    template <typename InputIterator>
    flat_set(InputIterator first, InputIterator last, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type())
      : _super{comp, alloc}
    {
        this->_initialize_container(first, last);
    }

    template <typename InputIterator>
    flat_set(InputIterator first, InputIterator last, allocator_type const& alloc)
      : _super{alloc}
    {
        this->_initialize_container(first, last);
    }

    flat_set(flat_set const& other) = default;
    flat_set(flat_set const& other, allocator_type const& alloc)
      : _super{other, alloc} { }

    flat_set(flat_set&& other) = default;
    flat_set(flat_set&& other, allocator_type const& alloc)
      : _super{std::move(other), alloc} { }

    flat_set(std::initializer_list<value_type> init, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type())
      : _super{comp, alloc}
    {
        this->_initialize_container(init.begin(), init.end());
    }

    flat_set(std::initializer_list<value_type> init, allocator_type const& alloc)
      : _super{alloc}
    {
        this->_initialize_container(init.begin(), init.end());
    }

    flat_set& operator=(flat_set const& other) = default;

    flat_set& operator=(flat_set&& other) noexcept(std::is_nothrow_move_assignable_v<_super>)
#if FLAT_MAP_WORKAROUND(FLAT_MAP_COMPILER_GCC, < FLAT_MAP_COMPILER_VERSION(10,0,0))
    {
        _super::operator=(std::move(other));
        return *this;
    }
#else
      = default;
#endif

    flat_set& operator=(std::initializer_list<value_type> ilist)
    {
        this->_initialize_container(ilist.begin(), ilist.end());
        return *this;
    }

    using _super::get_allocator;

    using _super::begin;
    using _super::cbegin;
    using _super::end;
    using _super::cend;
    using _super::rbegin;
    using _super::crbegin;
    using _super::rend;
    using _super::crend;

    using _super::empty;
    using _super::size;
    using _super::max_size;
    using _super::reserve;
    using _super::capacity;
    using _super::shrink_to_fit;
    using _super::clear;

    using _super::insert;
    using _super::emplace;
    using _super::emplace_hint;

    using _super::erase;

    void swap(flat_set& other) noexcept(noexcept(this->_super::swap(other))) { _super::swap(other); }

    using _super::extract;

    template <typename Comp, typename Allocator>
    void merge(std::set<key_type, Comp, Allocator>& source) { this->_merge(source, std::false_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::set<key_type, Comp, Allocator>&& source) { this->_merge(source, std::false_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::multiset<key_type, Comp, Allocator>& source) { this->_merge(source, std::true_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::multiset<key_type, Comp, Allocator>&& source) { this->_merge(source, std::true_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_set<key_type, Comp, Cont>& source) { this->_merge(source, std::false_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_set<key_type, Comp, Cont>&& source) { this->_merge(source, std::false_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_multiset<key_type, Comp, Cont>& source) { this->_merge(source, std::true_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_multiset<key_type, Comp, Cont>&& source) { this->_merge(source, std::true_type{}); }

    using _super::count;
    using _super::find;
    using _super::contains;
    using _super::equal_range;
    using _super::lower_bound;
    using _super::upper_bound;
    using _super::key_comp;
    using _super::value_comp;
};

template <typename Key, typename Compare, typename Container>
bool operator==(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

#ifndef FLAT_MAP_HAS_THREE_WAY_COMPARISON
template <typename Key, typename Compare, typename Container>
bool operator!=(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return !(lhs == rhs);
}

template <typename Key, typename Compare, typename Container>
bool operator<(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key, typename Compare, typename Container>
bool operator<=(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return !(rhs < lhs);
}

template <typename Key, typename Compare, typename Container>
bool operator>(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return rhs < lhs;
}

template <typename Key, typename Compare, typename Container>
bool operator>=(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return !(lhs < rhs);
}
#else
template <typename Key, typename Compare, typename Container>
auto operator<=>(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs)
{
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
#endif

template <typename Key, typename Compare, typename Container>
void swap(flat_set<Key, Compare, Container>& lhs, flat_set<Key, Compare, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

template <typename Key, typename Compare, typename Container, typename Pred>
constexpr typename flat_set<Key, Compare, Container>::size_type
erase_if(flat_set<Key, Compare, Container>& c, Pred pred)
{
    auto itr = std::remove_if(c.begin(), c.end(), std::forward<Pred>(pred));
    auto r = std::distance(itr, c.end());
    c.erase(itr, c.end());
    return r;
}

template <typename InputIterator,
          typename Key = typename std::iterator_traits<InputIterator>::value_type,
          typename Compare = std::less<typename std::iterator_traits<InputIterator>::value_type>,
          typename Allocator = typename std::vector<Key>::allocator_type,
          typename = std::enable_if_t<!detail::is_allocator_v<Compare>>,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_set(InputIterator, InputIterator, Compare = Compare(), Allocator = Allocator())
  -> flat_set<Key, Compare, std::vector<Key, Allocator>>;

template <typename Key,
          typename Compare = std::less<Key>,
          typename Allocator = typename std::vector<Key>::allocator_type,
          typename = std::enable_if_t<!detail::is_allocator_v<Compare>>,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_set(std::initializer_list<Key>, Compare = Compare(), Allocator = Allocator())
  -> flat_set<Key, Compare, std::vector<Key, Allocator>>;

template <typename InputIterator, typename Allocator,
          typename Key = typename std::iterator_traits<InputIterator>::value_type,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_set(InputIterator, InputIterator, Allocator)
  -> flat_set<Key, std::less<Key>, std::vector<Key, Allocator>>;

template <typename Key, typename Allocator,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_set(std::initializer_list<Key>, Allocator)
  -> flat_set<Key, std::less<Key>, std::vector<Key, Allocator>>;

} // namespace flat_map
