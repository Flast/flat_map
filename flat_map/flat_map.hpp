// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if __has_include(<compare>)
#include <compare>
#endif

#include "flat_map/__fwd.hpp"
#include "flat_map/__flat_tree.hpp"

namespace flat_map
{

template <typename Key, typename T,
          typename Compare = std::less<Key>,
          typename Container = std::vector<std::pair<Key, T>>>
class flat_map : private detail::flat_tree<flat_map<Key, T, Compare, Container>, Key, std::pair<Key, T>, Compare, Container>
{
    using _super = detail::flat_tree<flat_map, Key, std::pair<Key, T>, Compare, Container>;

    // To lookup private comparator
    friend _super;

public:
    using key_type = typename _super::key_type;
    using mapped_type = T;
    using value_type = typename _super::value_type;
    using size_type = typename _super::size_type;
    using difference_type = typename _super::difference_type;
    using key_compare = typename _super::key_compare;
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
    struct value_compare
    {
    protected:
        Compare c;

        value_compare(Compare c) : c{std::move(c)} { }

    public:
        bool operator()(value_type const& lhs, value_type const& rhs) const
        {
            return c(lhs.first, rhs.first);
        }
    };
    using insert_return_type = typename _super::insert_return_type;

private:
    struct _comparator final : value_compare
    {
        _comparator(Compare const& comp) : value_compare{comp} { }

        using value_compare::operator();

        template <typename K>
        auto operator()(value_type const& lhs, K const& rhs) const
        {
            return this->c(lhs.first, rhs);
        }

        template <typename K>
        auto operator()(K const& lhs, value_type const& rhs) const
        {
            return this->c(lhs, rhs.first);
        }
    };

    static auto& _key_extractor(value_type const& value) { return value.first; }

public:
    flat_map() = default;

    explicit flat_map(Compare const& comp, allocator_type const& alloc = allocator_type())
      : _super{comp, alloc} { }

    explicit flat_map(allocator_type const& alloc)
      : _super{alloc} { }

    template <typename InputIterator>
    flat_map(InputIterator first, InputIterator last, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type())
      : _super{first, last, comp, alloc} { }

    template <typename InputIterator>
    flat_map(InputIterator first, InputIterator last, allocator_type const& alloc)
      : _super{first, last, alloc} { }

    flat_map(flat_map const& other) = default;
    flat_map(flat_map const& other, allocator_type const& alloc)
      : _super{other, alloc} { }

    flat_map(flat_map&& other) = default;
    flat_map(flat_map&& other, allocator_type const& alloc)
      : _super{std::move(other), alloc} { }

    flat_map(std::initializer_list<value_type> init, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type())
      : _super{init, comp, alloc} { }

    flat_map(std::initializer_list<value_type> init, allocator_type const& alloc)
      : _super{init, alloc} { }

    flat_map& operator=(flat_map const& other) = default;

    flat_map& operator=(flat_map&& other) noexcept(std::is_nothrow_move_assignable_v<_super>) = default;

    flat_map& operator=(std::initializer_list<value_type> ilist)
    {
        _super::operator=(ilist);
        return *this;
    }

    using _super::get_allocator;

    mapped_type const& at(key_type const& key) const
    {
        auto itr = lower_bound(key);
        if (itr == this->end() || this->_vcomp()(key, *itr)) { throw std::out_of_range("no such key"); }
        return itr->second;
    }

    mapped_type& at(key_type const& key)
    {
        return const_cast<mapped_type&>(const_cast<flat_map const*>(this)->at(key));
    }

    mapped_type& operator[](key_type const& key) { return try_emplace(key).first->second; }
    mapped_type& operator[](key_type&& key) { return try_emplace(std::move(key)).first->second; }

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
    using _super::insert_sorted;

private:
    template <typename K, typename M>
    std::pair<iterator, bool> _insert_or_assign(K&& key, M&& obj)
    {
        static_assert(std::is_assignable_v<mapped_type&, M&&>);
        auto [itr, found] = this->_find(key);
        if (!found) { itr = this->_container_emplace(itr, std::forward<K>(key), std::forward<M>(obj)); }
        else { itr->second = std::forward<M>(obj); }
        return {itr, !found};
    }

    template <typename K, typename M>
    iterator _insert_or_assign(const_iterator hint, K&& key, M&& obj)
    {
        auto [itr, found] = this->_find(hint, key);
        if (!found) { itr = this->_container_emplace(itr, std::forward<K>(key), std::forward<M>(obj)); }
        else { itr->second = std::forward<M>(obj); }
        return itr;
    }

public:
    template <typename M>
    std::pair<iterator, bool> insert_or_assign(key_type const& key, M&& obj) { return _insert_or_assign(key, std::forward<M>(obj)); }

    template <typename M>
    std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj) { return _insert_or_assign(std::move(key), std::forward<M>(obj)); }

    template <typename M>
    iterator insert_or_assign(const_iterator hint, key_type const& key, M&& obj) { return _insert_or_assign(hint, key, std::forward<M>(obj)); }

    template <typename M>
    iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj) { return _insert_or_assign(hint, std::move(key), std::forward<M>(obj)); }

    using _super::emplace;
    using _super::emplace_hint;

private:
    template <typename K, typename... Args>
    std::pair<iterator, bool> _try_emplace(K&& key, Args&&... args)
    {
        auto [itr, found] = this->_find(key);
        if (!found)
        {
            itr = this->_container_emplace(itr,
                                           std::piecewise_construct,
                                           std::forward_as_tuple(std::forward<K>(key)),
                                           std::forward_as_tuple(std::forward<Args>(args)...));
        }
        return {itr, !found};
    }

    template <typename K, typename... Args>
    iterator _try_emplace(const_iterator hint, K&& key, Args&&... args)
    {
        auto [itr, found] = this->_find(hint, key);
        if (!found)
        {
            itr = this->_container_emplace(itr,
                                           std::piecewise_construct,
                                           std::forward_as_tuple(std::forward<K>(key)),
                                           std::forward_as_tuple(std::forward<Args>(args)...));
        }
        return itr;
    }

public:
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type const& key, Args&&... args) { return _try_emplace(key, std::forward<Args>(args)...); }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) { return _try_emplace(std::move(key), std::forward<Args>(args)...); }

    template <typename... Args>
    iterator try_emplace(const_iterator hint, key_type const& key, Args&&... args) { return _try_emplace(hint, key, std::forward<Args>(args)...); }

    template <typename... Args>
    iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) { return _try_emplace(hint, std::move(key), std::forward<Args>(args)...); }

    using _super::erase;

    void swap(flat_map& other) noexcept(noexcept(this->_super::swap(other))) { _super::swap(other); }

    using _super::extract;

private:
    template <typename Comp, typename Allocator>
    std::bool_constant<std::is_empty_v<key_compare> && std::is_same_v<key_compare, Comp>>
    _same_order(std::map<key_type, mapped_type, Comp, Allocator>&);

    template <typename Comp, typename Allocator>
    std::bool_constant<std::is_empty_v<key_compare> && std::is_same_v<key_compare, Comp>>
    _same_order(std::multimap<key_type, mapped_type, Comp, Allocator>&);

    template <typename Comp, typename Cont>
    std::bool_constant<std::is_empty_v<key_compare> && std::is_same_v<key_compare, Comp>>
    _same_order(flat_map<key_type, mapped_type, Comp, Cont>&);

public:
    template <typename Comp, typename Allocator>
    void merge(std::map<key_type, mapped_type, Comp, Allocator>& source) { this->_merge(source, std::false_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::map<key_type, mapped_type, Comp, Allocator>&& source) { this->_merge(source, std::false_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::multimap<key_type, mapped_type, Comp, Allocator>& source) { this->_merge(source, std::true_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::multimap<key_type, mapped_type, Comp, Allocator>&& source) { this->_merge(source, std::true_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_map<key_type, mapped_type, Comp, Cont>& source) { this->_merge(source, std::false_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_map<key_type, mapped_type, Comp, Cont>&& source) { this->_merge(source, std::false_type{}); }

    using _super::count;
    using _super::find;
    using _super::contains;
    using _super::equal_range;
    using _super::lower_bound;
    using _super::upper_bound;
    using _super::key_comp;
    using _super::value_comp;
};

template <typename Key, typename T, typename Compare, typename Container>
bool operator==(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

#if !(defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_three_way_comparison))
template <typename Key, typename T, typename Compare, typename Container>
bool operator!=(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return !(lhs == rhs);
}

template <typename Key, typename T, typename Compare, typename Container>
bool operator<(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key, typename T, typename Compare, typename Container>
bool operator<=(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return !(rhs < lhs);
}

template <typename Key, typename T, typename Compare, typename Container>
bool operator>(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return rhs < lhs;
}

template <typename Key, typename T, typename Compare, typename Container>
bool operator>=(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return !(lhs < rhs);
}
#else
#define FLAT_MAP_HAS_THREE_WAY_COMPARISON 1
template <typename Key, typename T, typename Compare, typename Container>
auto operator<=>(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
#endif

template <typename Key, typename T, typename Compare, typename Container>
void swap(flat_map<Key, T, Compare, Container>& lhs, flat_map<Key, T, Compare, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

template <typename Key, typename T, typename Compare, typename Container, typename Pred>
constexpr typename flat_map<Key, T, Compare, Container>::size_type
erase_if(flat_map<Key, T, Compare, Container>& c, Pred pred)
{
    auto itr = std::remove_if(c.begin(), c.end(), std::forward<Pred>(pred));
    auto r = std::distance(itr, c.end());
    c.erase(itr, c.end());
    return r;
}

namespace detail
{

template <typename InputIterator>
using iter_key_t = std::remove_const_t<typename std::iterator_traits<InputIterator>::value_type::first_type>;

template <typename InputIterator>
using iter_val_t = typename std::iterator_traits<InputIterator>::value_type::second_type;

// Use variadics for deducing defaulted allocator
template <typename InputIterator, typename... Args>
using iter_cont_t = std::vector<std::pair<iter_key_t<InputIterator>, iter_val_t<InputIterator>>, Args...>;

template <typename T, typename = void>
struct is_allocator : public std::false_type {};

template <typename T>
struct is_allocator<T, std::void_t<typename T::value_type, decltype(std::declval<T&>().allocate(std::size_t{}))>> : public std::true_type {};

template <typename T>
inline constexpr bool is_allocator_v = is_allocator<T>{};

} // namespace flat_map::detail

template <typename InputIterator,
          typename Compare = std::less<detail::iter_key_t<InputIterator>>,
          typename Allocator = typename detail::iter_cont_t<InputIterator>::allocator_type,
          typename = std::enable_if_t<!detail::is_allocator_v<Compare>>,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_map(InputIterator, InputIterator, Compare = Compare(), Allocator = Allocator())
  -> flat_map<detail::iter_key_t<InputIterator>, detail::iter_val_t<InputIterator>, Compare, detail::iter_cont_t<InputIterator, Allocator>>;

template <typename Key, typename T,
          typename Compare = std::less<Key>,
          typename Allocator = typename std::vector<std::pair<Key, T>>::allocator_type,
          typename = std::enable_if_t<!detail::is_allocator_v<Compare>>,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_map(std::initializer_list<std::pair<Key, T>>, Compare = Compare(), Allocator = Allocator())
  -> flat_map<Key, T, Compare, std::vector<std::pair<Key, T>, Allocator>>;

template <typename InputIterator, typename Allocator,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_map(InputIterator, InputIterator, Allocator)
  -> flat_map<detail::iter_key_t<InputIterator>, detail::iter_val_t<InputIterator>, std::less<detail::iter_key_t<InputIterator>>, detail::iter_cont_t<InputIterator, Allocator>>;

template <typename Key, typename T, typename Allocator,
          typename = std::enable_if_t<detail::is_allocator_v<Allocator>>>
flat_map(std::initializer_list<std::pair<Key, T>>, Allocator)
  -> flat_map<Key, T, std::less<Key>, std::vector<std::pair<Key, T>, Allocator>>;

} // namespace flat_map
