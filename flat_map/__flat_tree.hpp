// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include "flat_map/enum.hpp"

namespace flat_map::detail
{

template <typename Compare, typename = void>
struct comparator_store
{
    Compare _compare;

    comparator_store() = default;
    comparator_store(Compare const& comp) : _compare{comp} {}
    comparator_store(Compare&& comp) : _compare{std::move(comp)} {}

    auto& _comp() const { return _compare; }
    auto& _comp() { return _compare; }
};

template <typename Compare>
struct comparator_store<Compare, std::enable_if_t<std::is_class_v<Compare> && !std::is_final_v<Compare>>> : public Compare
{
    comparator_store() = default;
    comparator_store(Compare const& comp) : Compare{comp} {}
    comparator_store(Compare&& comp) : Compare{std::move(comp)} {}

    auto& _comp() const { return *static_cast<Compare const*>(this); }
    auto& _comp() { return *static_cast<Compare*>(this); }
};

template <typename Subclass, typename Key, typename ValueType, typename Compare, typename Container>
class _flat_tree_base : private detail::comparator_store<Compare>
{
public:
    Container _container;

public:
    using key_type = Key;
    using value_type = ValueType;
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

    using detail::comparator_store<Compare>::_comp;

    auto _vcomp() const { return static_cast<typename Subclass::_comparator>(key_comp()); }
    auto _veq() const
    {
        return [comp = _vcomp()](value_type const& lhs, value_type const& rhs)
        {
            return !comp(lhs, rhs) && !comp(rhs, lhs);
        };
    }

    template <typename InputIterator>
    void _initialize_container(InputIterator first, InputIterator last)
    {
        _container.assign(first, last);
        std::stable_sort(_container.begin(), _container.end(), _vcomp());
        if constexpr (Subclass::_is_uniq)
        {
            auto itr = std::unique(_container.begin(), _container.end(), _veq());
            _container.erase(itr, _container.end());
        }
    }

public:
    _flat_tree_base() = default;

    explicit _flat_tree_base(Compare const& comp, allocator_type const& alloc)
      : detail::comparator_store<Compare>{comp}, _container{alloc} { }

    explicit _flat_tree_base(allocator_type const& alloc)
      : _container{alloc} { }

    _flat_tree_base(_flat_tree_base const& other) = default;
    _flat_tree_base(_flat_tree_base const& other, allocator_type const& alloc)
      : detail::comparator_store<Compare>{other._comp()}, _container{other._container, alloc} { }

    _flat_tree_base(_flat_tree_base&& other) = default;
    _flat_tree_base(_flat_tree_base&& other, allocator_type const& alloc)
      : detail::comparator_store<Compare>{std::move(other._comp())}, _container{std::move(other._container), alloc} { }

    _flat_tree_base& operator=(_flat_tree_base const& other) = default;

    _flat_tree_base& operator=(_flat_tree_base&& other) noexcept(noexcept(_container = std::move(other._container)) && std::is_nothrow_move_assignable_v<Compare>) = default;

    allocator_type get_allocator() const noexcept { return _container.get_allocator(); }

    iterator begin() noexcept { return _container.begin(); }
    const_iterator begin() const noexcept { return _container.begin(); }
    const_iterator cbegin() const noexcept { return _container.cbegin(); }
    iterator end() noexcept { return _container.end(); }
    const_iterator end() const noexcept { return _container.end(); }
    const_iterator cend() const noexcept { return _container.cend(); }
    reverse_iterator rbegin() noexcept { return _container.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return _container.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return _container.crbegin(); }
    reverse_iterator rend() noexcept { return _container.rend(); }
    const_reverse_iterator rend() const noexcept { return _container.rend(); }
    const_reverse_iterator crend() const noexcept { return _container.crend(); }

    [[nodiscard]] bool empty() const noexcept { return _container.empty(); }
    size_type size() const noexcept { return _container.size(); }
    size_type max_size() const noexcept { return _container.max_size(); }
    // extension
    template <typename... ShouldBeEmpty, typename Dummy = Container>
    auto reserve(size_type new_cap) -> std::void_t<decltype(std::declval<Dummy>().reserve(new_cap))>
    {
        static_assert(sizeof...(ShouldBeEmpty) == 0);
        _container.reserve(new_cap);
    }
    // extension
    template <typename... ShouldBeEmpty, typename Dummy = Container>
    auto capacity() const noexcept -> decltype(std::declval<Dummy>().capacity(), size_type{})
    {
        static_assert(sizeof...(ShouldBeEmpty) == 0);
        return _container.capacity();
    }
    // extension
    template <typename... ShouldBeEmpty, typename Dummy = Container>
    auto shrink_to_fit() -> std::void_t<decltype(std::declval<Dummy>().shrink_to_fit())>
    {
        static_assert(sizeof...(ShouldBeEmpty) == 0);
        _container.shrink_to_fit();
    }
    void clear() noexcept { return _container.clear(); }

    template <typename K>
    std::pair<iterator, bool> _find(K const& key)
    {
        auto itr = lower_bound(key);
        return {itr, !(itr == end() || _vcomp()(key, *itr))};
    }

    template <typename V>
    auto _insert(V&& value)
    {
        if constexpr (Subclass::_is_uniq)
        {
            // It should be guaranteed that the value isn't changed when found
            auto [itr, found] = _find(Subclass::_key_extractor(value));
            if (!found) { itr = _container.insert(itr, std::forward<V>(value)); }
            return std::make_pair(itr, !found);
        }
        else
        {
            auto itr = upper_bound(Subclass::_key_extractor(value));
            return _container.insert(itr, std::forward<V>(value));
        }
    }

    auto insert(value_type const& value) { return _insert(value); }

    template <typename V>
    auto insert(V&& value) noexcept(std::enable_if_t<std::is_constructible_v<value_type, V&&>, std::false_type>{})
    {
        return _insert(value_type(std::forward<V>(value)));
    }

    auto insert(value_type&& value) { return _insert(std::move(value)); }

private:
    iterator _mutable(const_iterator itr)
    {
        return std::next(_container.begin(), std::distance(_container.cbegin(), itr));
    }

public:
    auto _insert_point_uniq(const_iterator hint, key_type const& key)
    {
        if (hint != end())
        {
            if (_vcomp()(key, *hint))
            {
                bool insert_here = hint == begin() || _vcomp()(*std::prev(hint), key); // 1
                if (!insert_here)
                {
                    hint = std::lower_bound(cbegin(), std::prev(hint), key, _vcomp());
                    bool found_insert_point = _vcomp()(key, *hint); // 2
                    if (!found_insert_point) { return std::make_pair(_mutable(hint), true); } // 3
                }
            }
            else
            {
                bool found_value = !_vcomp()(*hint, key);
                if (found_value) { return std::make_pair(_mutable(hint), true); } // 4

                hint = std::lower_bound(std::next(hint), cend(), key, _vcomp());
                bool found_insert_point = hint == end() || _vcomp()(key, *hint); // 5
                if (!found_insert_point) { return std::make_pair(_mutable(hint), true); } // 6
            }
        }
        else
        {
            bool insert_here = hint == begin() || _vcomp()(*std::prev(hint), key); // 7
            if (!insert_here) { return _find(key); } // 8
        }
        return std::make_pair(_mutable(hint), false);
    }

    auto _insert_point_multi(const_iterator hint, key_type const& key)
    {
        if (hint == end() || !_vcomp()(*hint, key))
        {
            bool insert_here = hint == begin() || !_vcomp()(key, *std::prev(hint)); // 1
            if (!insert_here)
            {
                hint = std::upper_bound(cbegin(), std::prev(hint), key, _vcomp()); // 2
            }
        }
        else
        {
            hint = std::lower_bound(std::next(hint), cend(), key, _vcomp()); // 3
        }
        return hint;
    }

    auto _insert_point(const_iterator hint, key_type const& key)
    {
        if constexpr (Subclass::_is_uniq) { return _insert_point_uniq(hint, key); }
        else { return _insert_point_multi(hint, key); }
    }

    template <typename V>
    iterator _insert(const_iterator hint, V&& value)
    {
        if constexpr (Subclass::_is_uniq)
        {
            auto [itr, found] = _insert_point(hint, Subclass::_key_extractor(value));
            if (!found) { itr = _container.insert(itr, std::forward<V>(value)); }
            return itr;
        }
        else
        {
            auto itr = _insert_point(hint, Subclass::_key_extractor(value));
            return _container.insert(itr, std::forward<V>(value));
        }
    }

    iterator insert(const_iterator hint, value_type const&& value) { return _insert(hint, value); }

    template <typename V>
    std::enable_if_t<std::is_constructible_v<value_type, V&&>, iterator>
    insert(const_iterator hint, V&& value) { return emplace_hint(hint, std::forward<V>(value)); }

    iterator insert(const_iterator hint, value_type&& value) { return _insert(hint, std::move(value)); }

    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last) { insert(range_order::no_ordered, first, last); }

    void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }
    // extension
    template <typename InputIterator>
    void insert(range_order order, InputIterator first, InputIterator last)
    {
        auto mid = _container.insert(_container.end(), first, last);
        switch (order)
        {
        case range_order::no_ordered:
        case range_order::uniqued:
            std::stable_sort(_container.begin(), _container.end(), _vcomp());
            break;

        case range_order::sorted:
        case range_order::unique_sorted:
            std::inplace_merge(_container.begin(), mid, _container.end(), _vcomp());
            break;
        }
        if constexpr (Subclass::_is_uniq)
        {
            auto itr = std::unique(_container.begin(), _container.end(), _veq());
            _container.erase(itr, _container.end());
        }
    }

    // extension
    void insert(range_order order, std::initializer_list<value_type> ilist) { insert(order, ilist.begin(), ilist.end()); }

    auto insert(node_type&& node)
    {
        if constexpr (Subclass::_is_uniq)
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
        else
        {
            if (!node.value.has_value()) { return end(); }
            return insert(std::move(*node.value));
        }
    }

    iterator insert(const_iterator hint, node_type&& node)
    {
        if (!node.value.has_value()) { return end(); }
        return _insert(hint, std::move(*node.value));
    }

    template <typename... Args>
    auto emplace(Args&&... args) { return _insert(value_type(std::forward<Args>(args)...)); }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) { return _insert(hint, value_type(std::forward<Args>(args)...)); }

    iterator erase(iterator pos) { return _container.erase(pos); }
    iterator erase(const_iterator first, const_iterator last) { return _container.erase(first, last); }

    size_type erase(key_type const& key)
    {
        auto itr = lower_bound(key);
        if (itr == end() || _vcomp()(key, *itr)) { return 0; }
        _container.erase(itr);
        return 1;
    }

    void swap(_flat_tree_base& other) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value && std::is_nothrow_swappable<Compare>::value)
    {
        using std::swap;
        swap(this->_comp(), other._comp());
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

    // Specialization for same order with stateless comparator
    template <typename Cont>
    void _merge_ordered_uniq(Cont& source, std::false_type /* multimap */)
    {
        auto first = source.begin();
        if (empty()) { goto out; }
        for (auto itr = begin(); first != source.end(); )
        {
            while (value_comp()(*itr, *first)) { if (++itr == end()) { goto out; } }
            if (value_comp()(*first, *itr))
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
    void _merge_ordered_uniq(Cont& source, std::true_type /* multimap */)
    {
        auto itr = begin();
        for (auto first = source.begin(); first != source.end(); )
        {
            while (itr != end() && value_comp()(*itr, *first)) { ++itr; }
            if (itr == end() || value_comp()(*first, *itr))
            {
                itr = _container.insert(itr, std::move(*first));
                first = source.erase(first);
                while (first != source.end() && !value_comp()(*itr, *first)) { ++first; } // skip duplicated
                ++itr;
            }
            else
            {
                ++first;
            }
        }
    }

    template <typename Cont, typename Cond>
    void _merge_unordered_uniq(Cont& source, Cond multimap)
    {
        for (auto first = source.begin(); first != source.end(); )
        {
            auto [itr, inserted] = _insert(*first);
            first = inserted ? source.erase(first) : std::next(first);
            if constexpr (multimap)
            {
                auto comp = source.value_comp();
                while (first != source.end() && !comp(*itr, *first)) { ++first; } // skip duplicated
            }
        }
    }

    template <typename Cont, typename Cond>
    void _merge(Cont& source, [[maybe_unused]] Cond multimap)
    {
        constexpr auto same_order = decltype(static_cast<Subclass*>(this)->_same_order(source)){};
        if constexpr (Subclass::_is_uniq)
        {
            if constexpr (same_order)
            {
                _merge_ordered_uniq(source, multimap);
            }
            else
            {
                _merge_unordered_uniq(source, multimap);
            }
        }
        else
        {
            constexpr auto order = same_order ? range_order::sorted : range_order::no_ordered;
            insert(order, std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
            source.clear();
        }
    }

private:
    template <typename K, typename U>
    using enable_if_transparent = std::enable_if_t<(sizeof(typename std::enable_if_t<(sizeof(K*) > 0), key_compare>::is_transparent*) > 0), U>;

public:
    template <typename K>
    size_type _count(K const& key) const
    {
        if constexpr (Subclass::_is_uniq)
        {
            return find(key) == end() ? 0 : 1;
        }
        else
        {
            auto [first, last] = equal_range(key);
            return std::distance(first, last);
        }
    }

    size_type count(key_type const& key) const { return _count(key); }

    template <typename K>
    enable_if_transparent<K, size_type> count(K const& key) const { return _count(key); }

    iterator find(key_type const& key)
    {
        auto [itr, found] = _find(key);
        return found ? itr : end();
    }

    const_iterator find(key_type const& key) const { return const_cast<_flat_tree_base*>(this)->find(key); }

    template <typename K>
    enable_if_transparent<K, iterator> find(K const& key)
    {
        auto [itr, found] = _find(key);
        return found ? itr : end();
    }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    find(K const& key) const { return const_cast<_flat_tree_base*>(this)->template find<K>(key); }

    bool contains(key_type const& key) const { return const_cast<_flat_tree_base*>(this)->_find(key).second; }

    template <typename K>
    enable_if_transparent<K, bool> contains(K const& key) { return _find(key).second; }

    template <typename K>
    std::pair<iterator, iterator> _equal_range(K const& key)
    {
        if constexpr (Subclass::_is_uniq)
        {
            auto itr = lower_bound(key);
            return {itr, (itr == end() || _vcomp()(key, *itr)) ? itr : std::next(itr)};
        }
        else
        {
            return std::equal_range(begin(), end(), key, _vcomp());
        }
    }

    std::pair<iterator, iterator> equal_range(key_type const& key) { return _equal_range(key); }

    std::pair<const_iterator, const_iterator> equal_range(key_type const& key) const { return const_cast<_flat_tree_base*>(this)->equal_range(key); }

    template <typename K>
    enable_if_transparent<K, std::pair<iterator, iterator>> equal_range(K const& key) { return _equal_range(key); }

    template <typename K>
    enable_if_transparent<K, std::pair<const_iterator, const_iterator>>
    equal_range(K const& key) const { return const_cast<_flat_tree_base*>(this)->template equal_range<K>(key); }

    iterator lower_bound(key_type const& key) { return std::lower_bound(begin(), end(), key, _vcomp()); }

    const_iterator lower_bound(key_type const& key) const { return const_cast<_flat_tree_base*>(this)->lower_bound(key); }

    template <typename K>
    enable_if_transparent<K, iterator> lower_bound(K const& key) { return std::lower_bound(begin(), end(), key, _vcomp()); }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    lower_bound(K const& key) const { return const_cast<_flat_tree_base*>(this)->template lower_bound<K>(key); }

    iterator upper_bound(key_type const& key) { return std::upper_bound(begin(), end(), key, _vcomp()); }

    const_iterator upper_bound(key_type const& key) const { return const_cast<_flat_tree_base*>(this)->upper_bound(key); }

    template <typename K>
    enable_if_transparent<K, iterator> upper_bound(K const& key) { return std::upper_bound(begin(), end(), key, _vcomp()); }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    upper_bound(K const& key) const { return const_cast<_flat_tree_base*>(this)->template upper_bound<K>(key); }

    key_compare key_comp() const { return this->_comp(); }
    auto value_comp() { return static_cast<typename Subclass::value_compare>(_vcomp()); }
};

} // namespace flat_map::detail
