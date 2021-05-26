// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "flat_map/__fwd.hpp"

namespace flat_map
{

template <typename Key, typename T,
          typename Compare = std::less<Key>,
          typename Container = std::vector<std::pair<Key, T>>>
class flat_map : private Compare
{
    Container _container;

public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<Key, T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using key_compare = Compare;
    using allocator_type = typename Container::allocator_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using pointer = typename Container::pointer;
    using const_pointer = typename Container::const_pointer;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using reverse_iterator = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;
    struct node_type
    {
        std::optional<value_type> value;

        node_type() = default;
        node_type(value_type&& value) : value{std::move(value)} {}
    };
    struct insert_return_type
    {
        iterator position;
        bool inserted;
        node_type node;
    };

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

private:
    decltype(auto) _comp() const { return *static_cast<Compare const*>(this); }
    decltype(auto) _comp() { return *static_cast<Compare*>(this); }

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

    auto _vcomp() const { return _comparator{_comp()}; }
    auto _evcomp() const
    {
        return [this](value_type const& lhs, value_type const& rhs)
        {
            return !_comp()(lhs.first, rhs.first) && !_comp()(rhs.first, lhs.first);
        };
    }

    void _initialize_container()
    {
        std::sort(_container.begin(), _container.end(), _vcomp());
        auto itr = std::unique(_container.begin(), _container.end(), _evcomp());
        _container.erase(itr, _container.end());
    }

public:
    flat_map() = default;

    explicit flat_map(Compare const& comp, allocator_type const& alloc = allocator_type())
      : Compare{comp}, _container{alloc} { }

    explicit flat_map(allocator_type const& alloc)
      : _container{alloc} { }

    template <typename InputIterator>
    flat_map(InputIterator first, InputIterator last, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type())
      : Compare{comp}, _container{first, last, alloc}
    {
        _initialize_container();
    }

    template <typename InputIterator>
    flat_map(InputIterator first, InputIterator last, allocator_type const& alloc)
      : _container{first, last, alloc}
    {
        _initialize_container();
    }

    flat_map(flat_map const& other) = default;
    flat_map(flat_map const& other, allocator_type const& alloc)
      : Compare{other._comp()}, _container{other._container, alloc} { }

    flat_map(flat_map&& other) = default;
    flat_map(flat_map&& other, allocator_type const& alloc)
      : Compare{std::move(other._comp())}, _container{std::move(other._container), alloc} { }

    flat_map(std::initializer_list<value_type> init, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type())
      : Compare{comp}, _container{init, alloc}
    {
        _initialize_container();
    }

    flat_map(std::initializer_list<value_type> init, allocator_type const& alloc)
      : _container{init, alloc}
    {
        _initialize_container();
    }

    flat_map& operator=(flat_map const& other) = default;

    flat_map& operator=(flat_map&& other) noexcept(noexcept(_container = std::move(other._container)) && std::is_nothrow_move_assignable_v<Compare>) = default;

    flat_map& operator=(std::initializer_list<value_type> ilist)
    {
        _container = ilist;
        _initialize_container();
        return *this;
    }

    allocator_type get_allocator() const noexcept { return _container.get_allocator(); }

    mapped_type const& at(key_type const& key) const
    {
        auto itr = lower_bound(key);
        if (itr == _container.end() || _comp()(key, itr->first)) { throw std::out_of_range("no such key"); }
        return itr->second;
    }

    mapped_type& at(key_type const& key)
    {
        return const_cast<mapped_type&>(const_cast<flat_map const*>(this)->at(key));
    }

    mapped_type& operator[](key_type const& key) { return try_emplace(key).first->second; }
    mapped_type& operator[](key_type&& key) { return try_emplace(std::move(key)).first->second; }

    auto begin() noexcept { return _container.begin(); }
    auto begin() const noexcept { return _container.begin(); }
    auto cbegin() const noexcept { return _container.cbegin(); }
    auto end() noexcept { return _container.end(); }
    auto end() const noexcept { return _container.end(); }
    auto cend() const noexcept { return _container.cend(); }
    auto rbegin() noexcept { return _container.rbegin(); }
    auto rbegin() const noexcept { return _container.rbegin(); }
    auto crbegin() const noexcept { return _container.crbegin(); }
    auto rend() noexcept { return _container.rend(); }
    auto rend() const noexcept { return _container.rend(); }
    auto crend() const noexcept { return _container.crend(); }

    [[nodiscard]] bool empty() const noexcept { return _container.empty(); }
    size_type size() const noexcept { return _container.size(); }
    size_type max_size() const noexcept { return _container.max_size(); }
    void clear() noexcept { return _container.clear(); }

private:
    template <typename K>
    std::pair<iterator, bool> _find(K const& key)
    {
        auto itr = lower_bound(key);
        return {itr, !(itr == end() || _comp()(key, itr->first))};
    }

    // It should be guaranteed that the value isn't changed when found
    template <typename V>
    std::pair<iterator, bool> _insert(V&& value)
    {
        auto [itr, found] = _find(value.first);
        if (!found) { itr = _container.insert(itr, std::forward<V>(value)); }
        return {itr, !found};
    }

public:
    std::pair<iterator, bool> insert(value_type const& value) { return _insert(value); }

    template <typename V>
    std::enable_if_t<std::is_constructible_v<value_type, V&&>, std::pair<iterator, bool>>
    insert(V&& value) { return emplace(std::forward<V>(value)); }

    std::pair<iterator, bool> insert(value_type&& value) { return _insert(std::move(value)); }

private:
    iterator _mutable(const_iterator itr)
    {
        return std::next(_container.begin(), std::distance(_container.cbegin(), itr));
    }

    std::pair<iterator, bool> _find(const_iterator hint, key_type const& key)
    {
        if (hint != end())
        {
            if (_comp()(key, hint->first))
            {
                bool insert_here = hint == begin() || _comp()(std::prev(hint)->first, key); // 1
                if (!insert_here)
                {
                    hint = std::lower_bound(cbegin(), std::prev(hint), key, _vcomp());
                    bool found_insert_point = _comp()(key, hint->first); // 2
                    if (!found_insert_point) { return {_mutable(hint), true}; } // 3
                }
            }
            else
            {
                bool found_value = !_comp()(hint->first, key);
                if (found_value) { return {_mutable(hint), true}; } // 4

                hint = std::lower_bound(std::next(hint), cend(), key, _vcomp());
                bool found_insert_point = hint == end() || _comp()(key, hint->first); // 5
                if (!found_insert_point) { return {_mutable(hint), true}; } // 6
            }
        }
        else
        {
            bool insert_here = hint == begin() || _comp()(std::prev(hint)->first, key); // 7
            if (!insert_here) { return _find(key); } // 8
        }
        return {_mutable(hint), false};
    }

    template <typename V>
    iterator _insert(const_iterator hint, V&& value)
    {
        auto [itr, found] = _find(hint, value.first);
        if (!found) { itr = _container.insert(itr, std::forward<V>(value)); }
        return itr;
    }

public:
    iterator insert(const_iterator hint, value_type const&& value) { return _insert(hint, value); }

    template <typename V>
    std::enable_if_t<std::is_constructible_v<value_type, V&&>, iterator>
    insert(const_iterator hint, V&& value) { return emplace_hint(hint, std::forward<V>(value)); }

    iterator insert(const_iterator hint, value_type&& value) { return _insert(hint, std::move(value)); }

    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        while (first != last) { _insert(*first++); }
    }

    void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }

private:
    template <typename ForwardIterator>
    void _insert_sorted(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag)
    {
        for (auto itr = begin(); first != last && itr != end(); )
        {
            while (_comp()(itr->first, first->first)) { ++itr; }

            auto orig = first++; // declare here to advance first anyway
            if (_comp()(orig->first, itr->first))
            {
                // find unique range
                for (; first != last && _comp()(std::prev(first)->first, first->first) && _comp()(first->first, itr->first); ++first);

                itr = _container.insert(itr, orig, first);
                std::advance(itr, std::distance(orig, first) - 1); // sub 1 to compare *--first and *first (to skip duplicated)
            }
        }
        _container.insert(end(), first, last); // insert remainings
    }

    template <typename InputIterator>
    void _insert_sorted(InputIterator first, InputIterator last, std::input_iterator_tag)
    {
        for (auto itr = begin(); first != last && itr != end(); ++first)
        {
            while (_comp()(itr->first, first->first)) { ++itr; }
            if (_comp()(first->first, itr->first))
            {
                itr = _container.insert(itr, *first);
                // Don't advance iterator to compare *first and *++first (to skip duplicated).
            }
        }
        _container.insert(end(), first, last); // insert remainings
    }

public:
    // extension
    template <typename InputIterator>
    void insert_sorted(InputIterator first, InputIterator last)
    {
        _insert_sorted(first, last, typename std::iterator_traits<InputIterator>::iterator_category{});
    }

    // extension
    void insert_sorted(std::initializer_list<value_type> ilist) { insert_sorted(ilist.begin(), ilist.end()); }

    insert_return_type insert(node_type&& node)
    {
        if (!node.value.has_value()) { return insert_return_type{end(), false, {}}; }
        if (auto [itr, inserted] = _insert(std::move(*node.value)); inserted)
        {
            return insert_return_type{itr, true, {}};
        }
        else
        {
            return insert_return_type{itr, false, std::move(node)};
        }
    }

    iterator insert(const_iterator hint, node_type&& node)
    {
        if (!node.value.has_value()) { return end(); }
        return _insert(hint, std::move(*node.value));
    }

private:
    template <typename K, typename M>
    std::pair<iterator, bool> _insert_or_assign(K&& key, M&& obj)
    {
        static_assert(std::is_assignable_v<mapped_type&, M&&>);
        auto [itr, found] = _find(key);
        if (!found) { itr = _container.emplace(itr, std::forward<K>(key), std::forward<M>(obj)); }
        else { itr->second = std::forward<M>(obj); }
        return {itr, !found};
    }

    template <typename K, typename M>
    iterator _insert_or_assign(const_iterator hint, K&& key, M&& obj)
    {
        auto [itr, found] = _find(hint, key);
        if (!found) { itr = _container.emplace(itr, std::forward<K>(key), std::forward<M>(obj)); }
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

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) { return _insert(value_type(std::forward<Args>(args)...)); }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) { return _insert(hint, value_type(std::forward<Args>(args)...)); }

private:
    template <typename K, typename... Args>
    std::pair<iterator, bool> _try_emplace(K&& key, Args&&... args)
    {
        auto [itr, found] = _find(key);
        if (!found)
        {
            itr = _container.emplace(itr,
                                     std::piecewise_construct,
                                     std::forward_as_tuple(std::forward<K>(key)),
                                     std::forward_as_tuple(std::forward<Args>(args)...));
        }
        return {itr, !found};
    }

    template <typename K, typename... Args>
    iterator _try_emplace(const_iterator hint, K&& key, Args&&... args)
    {
        auto [itr, found] = _find(hint, key);
        if (!found)
        {
            itr = _container.emplace(itr,
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

    iterator erase(iterator pos) { return _container.erase(pos); }
    iterator erase(const_iterator first, const_iterator last) { return _container.erase(first, last); }

    size_type erase(key_type const& key)
    {
        auto itr = lower_bound(key);
        if (itr == end() || _comp()(key, itr->first)) { return 0; }
        _container.erase(itr);
        return 1;
    }

    void swap(flat_map& other) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value && std::is_nothrow_swappable<Compare>::value)
    {
        using std::swap;
        swap(_comp(), other._comp());
        swap(_container, other._container);
    }

    node_type extract(const_iterator position)
    {
        // standard requires `valid dereferenceable constant iterator`
        assert(position != cend());

        node_type node{std::move(*_mutable(position))};
        erase(position);
        return node;
    }

    node_type extract(key_type const& key)
    {
        if (auto [itr, found] = _find(key); found)
        {
            node_type node{std::move(*itr)};
            erase(itr);
            return node;
        }
        return {};
    }

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

    // Specialization for same order with stateless comparator
    template <typename Cont>
    void _merge_ordered(Cont& source, std::false_type /* multimap */)
    {
        auto first = source.begin();
        if (empty()) { goto out; }
        for (auto itr = begin(); first != source.end(); )
        {
            while (_comp()(itr->first, first->first)) { if (++itr == end()) { goto out; } }
            if (_comp()(first->first, itr->first))
            {
                itr = std::next(_container.insert(itr, std::move(*first)));
                first = source.erase(first);
            }
            else
            {
                ++first;
            }
        }
    out:
        _container.insert(end(), std::make_move_iterator(first), std::make_move_iterator(source.end())); // insert remainings
        source.erase(first, source.end());
    }

    template <typename Cont>
    void _merge_ordered(Cont& source, std::true_type /* multimap */)
    {
        auto itr = begin();
        for (auto first = source.begin(); first != source.end(); )
        {
            while (itr != end() && _comp()(itr->first, first->first)) { ++itr; }
            if (itr == end() || _comp()(first->first, itr->first))
            {
                itr = _container.insert(itr, std::move(*first));
                first = source.erase(first);
                while (first != source.end() && !_comp()(itr->first, first->first)) { ++first; } // skip duplicated
                ++itr;
            }
            else
            {
                ++first;
            }
        }
    }

    template <typename Cont>
    void _merge_unordered(Cont& source, std::false_type /* multimap */)
    {
        for (auto first = source.begin(); first != source.end(); )
        {
            if (auto [itr, found] = _find(first->first); !found)
            {
                _container.insert(itr, std::move(*first));
                first = source.erase(first);
            }
            else
            {
                ++first;
            }
        }
    }

    template <typename Cont>
    void _merge_unordered(Cont& source, std::true_type /* multimap */)
    {
        auto comp = source.key_comp();
        for (auto first = source.begin(); first != source.end(); )
        {
            if (auto [itr, found] = _find(first->first); !found)
            {
                itr = _container.insert(itr, std::move(*first));
                first = source.erase(first);
                while (first != source.end() && !comp(itr->first, first->first)) { ++first; } // skip duplicated
            }
            else
            {
                ++first;
            }
        }
    }

    template <typename Cont, typename Cond>
    void _merge(Cont& source, Cond multimap)
    {
        if constexpr (decltype(_same_order(source)){})
        {
            _merge_ordered(source, multimap);
        }
        else
        {
            _merge_unordered(source, multimap);
        }
    }

public:
    template <typename Comp, typename Allocator>
    void merge(std::map<key_type, mapped_type, Comp, Allocator>& source) { _merge(source, std::false_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::map<key_type, mapped_type, Comp, Allocator>&& source) { _merge(source, std::false_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::multimap<key_type, mapped_type, Comp, Allocator>& source) { _merge(source, std::true_type{}); }

    template <typename Comp, typename Allocator>
    void merge(std::multimap<key_type, mapped_type, Comp, Allocator>&& source) { _merge(source, std::true_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_map<key_type, mapped_type, Comp, Cont>& source) { _merge(source, std::false_type{}); }

    // extension
    template <typename Comp, typename Cont>
    void merge(flat_map<key_type, mapped_type, Comp, Cont>&& source) { _merge(source, std::false_type{}); }

private:
    template <typename K, typename U>
    using enable_if_transparent = std::enable_if_t<(sizeof(typename std::enable_if_t<(sizeof(K*) > 0), key_compare>::is_transparent*) > 0), U>;

public:
    size_type count(key_type const& key) const { return find(key) == end() ? 0 : 1; }

    template <typename K>
    enable_if_transparent<K, size_type>
    count(K const& key) const { return find<K>(key) == end() ? 0 : 1; }

    iterator find(key_type const& key)
    {
        auto [itr, found] = _find(key);
        return found ? itr : end();
    }

    const_iterator find(key_type const& key) const { return const_cast<flat_map*>(this)->find(key); }

    template <typename K>
    enable_if_transparent<K, iterator> find(K const& key)
    {
        auto [itr, found] = _find(key);
        return found ? itr : end();
    }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    find(K const& key) const { return const_cast<flat_map*>(this)->template find<K>(key); }

    bool contains(key_type const& key) const { return count(key) != 0; }

    template <typename K>
    enable_if_transparent<K, bool>
    contains(K const& key) { return count<K>(key) != 0; }

    std::pair<iterator, iterator> equal_range(key_type const& key)
    {
        auto itr = lower_bound(key);
        return {itr, (itr == end() || _comp()(key, itr->first)) ? itr : std::next(itr)};
    }

    std::pair<const_iterator, const_iterator> equal_range(key_type const& key) const { return const_cast<flat_map*>(this)->equal_range(key); }

    template <typename K>
    enable_if_transparent<K, std::pair<iterator, iterator>> equal_range(K const& key)
    {
        auto itr = lower_bound(key);
        return {itr, (itr == end() || _comp()(key, itr->first)) ? itr : std::next(itr)};
    }

    template <typename K>
    enable_if_transparent<K, std::pair<const_iterator, const_iterator>>
    equal_range(K const& key) const { return const_cast<flat_map*>(this)->template equal_range<K>(key); }

    iterator lower_bound(key_type const& key) { return std::lower_bound(begin(), end(), key, _vcomp()); }

    const_iterator lower_bound(key_type const& key) const { return const_cast<flat_map*>(this)->lower_bound(key); }

    template <typename K>
    enable_if_transparent<K, iterator> lower_bound(K const& key) { return std::lower_bound(begin(), end(), key, _vcomp()); }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    lower_bound(K const& key) const { return const_cast<flat_map*>(this)->template lower_bound<K>(key); }

    iterator upper_bound(key_type const& key) { return std::upper_bound(begin(), end(), key, _vcomp()); }

    const_iterator upper_bound(key_type const& key) const { return const_cast<flat_map*>(this)->upper_bound(key); }

    template <typename K>
    enable_if_transparent<K, iterator> upper_bound(K const& key) { return std::upper_bound(begin(), end(), key, _vcomp()); }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    upper_bound(K const& key) const { return const_cast<flat_map*>(this)->template upper_bound<K>(key); }

    key_compare key_comp() const { return _comp(); }
    value_compare value_comp() const { return _vcomp(); }
};

template <typename Key, typename T, typename Compare, typename Container>
bool operator==(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

// #if !(defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_three_way_comparison))
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
// #else
// template <typename Key, typename T, typename Compare, typename Container>
// TBD operator<=>(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
// {
//     // TODO
// }
// #endif

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
