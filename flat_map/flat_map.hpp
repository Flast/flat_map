// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION
#   define FLATMAP_BEGIN_STD _LIBCPP_BEGIN_NAMESPACE_STD
#   define FLATMAP_END_STD _LIBCPP_END_NAMESPACE_STD
#else
#   define FLATMAP_BEGIN_STD namespace std {
#   define FLATMAP_END_STD }
#endif

FLATMAP_BEGIN_STD
template <typename Key, typename T, typename Compare, typename Container> class map;
template <typename Key, typename T, typename Compare, typename Container> class multimap;
FLATMAP_END_STD

#undef FLATMAP_BEGIN_STD
#undef FLATMAP_END_STD

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
    // TODO: node_type
    // TODO: insert_return_type

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

    auto _vcomp() const
    {
        struct comparator final : value_compare
        {
            comparator(Compare const& comp) : value_compare{comp} { }

            using value_compare::operator();

            auto operator()(value_type const& lhs, key_type const& rhs) const
            {
                return this->c(lhs.first, rhs);
            }

            auto operator()(key_type const& lhs, value_type const& rhs) const
            {
                return this->c(lhs, rhs.first);
            }
        };
        return comparator{_comp()};
    }
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

    flat_map& operator=(flat_map const& other)
    {
        _container = other._container;
        return *this;
    }

    flat_map& operator=(flat_map&& other) noexcept(noexcept(_container = std::move(other._container)) && std::is_nothrow_move_assignable_v<Compare>)
    {
        _container = std::move(other._container);
        return *this;
    }

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
    template <typename V>
    std::pair<iterator, bool> _insert(V&& value)
    {
        auto itr = lower_bound(value.first);
        if (itr == end() || _comp()(value.first, itr->first))
        {
            return {_container.insert(itr, std::forward<V>(value)), true};
        }
        return {itr, false};
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

    template <typename V>
    iterator _insert(const_iterator hint, V&& value)
    {
        if (hint != end())
        {
            if (_comp()(value.first, hint->first))
            {
                bool insert_here = hint == begin() || _comp()(std::prev(hint)->first, value.first); // 1
                if (!insert_here)
                {
                    hint = std::lower_bound(cbegin(), std::prev(hint), value, _vcomp());
                    bool found_insert_point = _comp()(value.first, hint->first); // 2
                    if (!found_insert_point) { return _mutable(hint); } // 3
                }
            }
            else
            {
                bool found_value = !_comp()(hint->first, value.first);
                if (found_value) { return _mutable(hint); } // 4

                hint = std::lower_bound(std::next(hint), cend(), value, _vcomp());
                bool found_insert_point = hint == end() || _comp()(value.first, hint->first); // 5
                if (!found_insert_point) { return _mutable(hint); } // 6
            }
        }
        else
        {
            bool insert_here = hint == begin() || _comp()(std::prev(hint)->first, value.first); // 7
            if (!insert_here) { return _insert(std::forward<V>(value)).first; } // 8
        }
        return _container.insert(hint, std::forward<V>(value));
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

    // TODO
    // insert_return_type insert(node_type&& node)
    // {
    //     // TODO
    // }

    // iterator insert(const_iterator hint, node_type&& node)
    // {
    //     // TODO
    // }

    template <typename M>
    std::pair<iterator, bool> insert_or_assign(key_type const& key, M&& obj)
    {
        // TODO
    }

    template <typename M>
    std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj)
    {
        // TODO
    }

    template <typename M>
    iterator insert_or_assign(const_iterator hint, key_type const& key, M&& obj)
    {
        // TODO
    }

    template <typename M>
    iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj)
    {
        // TODO
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        // TODO
    }

    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        // TODO
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type const& key, Args&&... args)
    {
        // TODO
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args)
    {
        // TODO
    }

    template <typename... Args>
    iterator try_emplace(const_iterator hint, key_type const& key, Args&&... args)
    {
        // TODO
    }

    template <typename... Args>
    iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args)
    {
        // TODO
    }

    iterator erase(iterator pos) { return _container.erase(pos); }
    iterator erase(const_iterator first, const_iterator last) { return _container.erase(first, last); }

    size_type erase(key_type const& key)
    {
        // TODO
    }

    void swap(flat_map& other) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value && std::is_nothrow_swappable<Compare>::value)
    {
        using std::swap;
        swap(_comp(), other._comp());
        swap(_container, other._container);
    }

    // TODO
    // node_type extract(const_iterator position)
    // {
    //     // TODO
    // }

    // node_type extract(key_type const& key)
    // {
    //     // TODO
    // }

    template <typename Comp>
    void merge(std::map<key_type, mapped_type, Comp, allocator_type>& source)
    {
        // TODO
    }

    template <typename Comp>
    void merge(std::map<key_type, mapped_type, Comp, allocator_type&>& source)
    {
        // TODO
    }

    template <typename Comp>
    void merge(std::multimap<key_type, mapped_type, Comp, allocator_type>& source)
    {
        // TODO
    }

    template <typename Comp>
    void merge(std::multimap<key_type, mapped_type, Comp, allocator_type&>& source)
    {
        // TODO
    }

    // // extension
    // template <typename Comp, typename Container,
    //           typename = std::enable_if_t<std::is_same<allocator_type, typename flat_map<key_type, mapped_type, Comp, Container>::allocator_type>>
    // void merge(flat_map<key_type, mapped_type, Comp, Container>& source)
    // {
    //     // TODO
    // }

    // // extension
    // template <typename Comp, typename Container,
    //           typename = std::enable_if_t<std::is_same<allocator_type, typename flat_map<key_type, mapped_type, Comp, Container>::allocator_type>>
    // void merge(flat_map<key_type, mapped_type, Comp, Container&>& source)
    // {
    //     // TODO
    // }

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
        auto itr = lower_bound(key);
        return (itr == end() || _comp()(key, itr->first)) ? end() : itr;
    }

    const_iterator find(key_type const& key) const { return const_cast<flat_map*>(this)->find(key); }

    template <typename K>
    enable_if_transparent<K, iterator>
    find(K const& key)
    {
        // TODO
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
    enable_if_transparent<K, std::pair<iterator, iterator>>
    equal_range(K const& key)
    {
        // TODO
    }

    template <typename K>
    enable_if_transparent<K, std::pair<const_iterator, const_iterator>>
    equal_range(K const& key) const { return const_cast<flat_map*>(this)->template equal_range<K>(key); }

    iterator lower_bound(key_type const& key) { return std::lower_bound(begin(), end(), key, _vcomp()); }

    const_iterator lower_bound(key_type const& key) const { return const_cast<flat_map*>(this)->lower_bound(key); }

    template <typename K>
    enable_if_transparent<K, iterator>
    lower_bound(K const& key)
    {
        // TODO
    }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    lower_bound(K const& key) const { return const_cast<flat_map*>(this)->template lower_bound<K>(key); }

    iterator upper_bound(key_type const& key) { return std::upper_bound(begin(), end(), key, _vcomp()); }

    const_iterator upper_bound(key_type const& key) const { return const_cast<flat_map*>(this)->upper_bound(key); }

    template <typename K>
    enable_if_transparent<K, iterator>
    upper_bound(K const& key)
    {
        // TODO
    }

    template <typename K>
    enable_if_transparent<K, const_iterator>
    upper_bound(K const& key) const { return const_cast<flat_map*>(this)->template upper_bound<K>(key); }

    key_compare key_comp() const { return _comp(); }
    value_compare value_comp() const { return _vcomp(); }
};

template <typename Key, typename T, typename Compare, typename Container>
bool operator==(flat_map<Key, T, Compare, Container> const& lhs, flat_map<Key, T, Compare, Container> const& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), lhs.begin(), rhs.end());
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
constexpr auto erase_if(flat_map<Key, T, Compare, Container>& c, Pred pred) -> typename decltype(c)::size_type
{
    auto itr = std::remove_if(c.begin(), c.end(), std::forward<Pred>(pred));
    auto r = std::distance(itr, c.end());
    c.erase(itr, c.end());
    return r;
}

} // namespace flat_map
