# flat\_set

```cpp
#include <flat_map/flat_set.hpp>

template <typename Key,
          typename Compare = std::less<Key>,
          typename Container = std::vector<Key>>
class flat_set;
```

**Requirements**
- `Container` should meet [*Container*](https://en.cppreference.com/w/cpp/named_req/SequenceContainer), [*AllocatorAwareContainer*](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer), [*SequenceContainer*](https://en.cppreference.com/w/cpp/named_req/SequenceContainer), and [*ReversibleContainer*](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer).

**Complexity**
- `N` denotes number of elements that stored in the container.
- `M` denotes number of elements that shifted by insert/erase.
- `E` denotes number of target elements that inserted or erased.

## Member types

```cpp
using key_type = Key;
using value_type = Key;
using size_type = std::size_t;
using difference_type = std::ptrdiff_t;
using key_compare = Compare;
using value_compare = Compare;
using allocator_type = typename Container::allocator_type;
using reference = typename Container::reference;
using const_reference = typename Container::const_reference;
using pointer = typename Container::pointer;
using const_pointer = typename Container::const_pointer;
using iterator = typename Container::iterator;
using const_iterator = typename Container::const_iterator;
using reverse_iterator = typename Container::reverse_iterator;
using const_reverse_iterator = typename Container::const_reverse_iterator;
using node_type = /* unspecified */;
using insert_return_type = struct /* unspecified */
{
    iterator  position;
    bool      inserted;
    node_type node;
};
```

## Constructors

```cpp
flat_set();

explicit flat_set(Compare const& comp, allocator_type const& alloc = allocator_type());
```

```cpp
template <typename InputIterator>
flat_set(InputIterator first, InputIterator last, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type());

template <typename InputIterator>
flat_set(InputIterator first, InputIterator last, allocator_type const& alloc);
```

Construct container from `[first, last)`.

**Pre requirements**
`InputIterator` should meet [*InputIterator*](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator).

**Complexity**
`O(E log(E))` if enough additional memory is available, otherwise `O(E log(E)^2)`.

```cpp
flat_set(flat_set const& other);

flat_set(flat_set const& other, allocator_type const& alloc);
```

Copy from other.

```cpp
flat_set(flat_set&& other);

flat_set(flat_set&& other, allocator_type const& alloc);
```

Move entire elements from other.

```cpp
flat_set(std::initializer_list<value_type> init, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type());

flat_set(std::initializer_list<value_type> init, allocator_type const& alloc);
```

Construct from init.

**Complexity**
`O(E log(E))` if enough additional memory is available, otherwise `O(E log(E)^2)`.

## Assignments

```cpp
flat_set& operator=(flat_set const& other);
```

Copy from other.

```cpp
flat_set& operator=(flat_set&& other) noexcept(/* see below */);
```

Move entire elements from other.

**Exceptions**
No except only if it meets all of
- `std::is_nothrow_move_assignable_v<Container> == true` and
- `std::is_nothrow_move_assignable_v<Compare> == true`

```cpp
flat_set& operator=(std::initializer_list<value_type> ilist);
```

**Complexity**
`O(E log(E))` if enough additional memory is available, otherwise `O(E log(E)^2)`.

## Iterators

### begin

```cpp
iterator begin() noexcept;
const_iterator begin() const noexcept;
```

### cbegin

```cpp
const_iterator cbegin() const noexcept;
```

### end

```cpp
iterator end() noexcept;
const_iterator end() const noexcept;
```

### cend

```cpp
const_iterator cend() const noexcept;
```

### rbegin

```cpp
reverse_iterator rbegin() noexcept;
const_reverse_iterator rbegin() const noexcept;
```

### crbegin

```cpp
const_reference crbegin() const noexcept;
```

### rend

```cpp
reverse_iterator rend() noexcept;
const_reverse_iterator rend() const noexcept;
```

### crend

```cpp
const_reference crend() const noexcept;
```

## Capacity

### empty

```cpp
bool empty() const noexcept;
```

### size

```cpp
size_type size() const noexcept;
```

### max_size

```cpp
size_type max_size() const noexcept;
```

### reserve

```cpp
void reserve(size_type new_cap);
```

This function is provided only if `Container::reserve()` is provided.

**Postcondition**
- `capacity() == new_cap`

### capacity

```cpp
size_type capacity() const noexcept;
```

This function is provided only if `Container::capacity()` is provided.

### shrink_to_fit

```cpp
void shrink_to_fit();
```

This function is provided only if `Container::shrink_to_fit()` is provided.

**Postcondition**
- `capacity() == size()`

**Complexity**
`O(N)`.

## Modifiers

### clear

```cpp
void clear();
```

Clear container.

**Postcondition**
- `size() == 0`

**Invalidation**
Invalidates every interators and references.

### insert

```cpp
std::pair<iterator, bool> insert(value_type const& value);

template <typename V>
std::pair<iterator, bool> insert(V&& value);

std::pair<iterator, bool> insert(value_type&& value);
```

Insert a `value`.

The second form only participants in overload resolution if `std::is_constructible_v<value_type, V&&> == true`.

**Return value**
An iterator to inserted value or the element which has same key, `bool` denotes whether the insertion is succeeded.

**Complexity**
Amortized `O(M)` for insertion, `O(log(N))` for searching insertion point.

**Invalidation**
Same as `Container::insert`.

```cpp
iterator insert(const_iterator hint, value_type const&& value);

template <typename V>
iterator insert(const_iterator hint, V&& value);

iterator insert(const_iterator hint, value_type&& value);
```

Insert a `value`.
The `hint` is used for looking up insertion point.

The second form only participants in overload resolution if `std::is_constructible_v<value_type, V&&> == true`.

**Return value**
An iterator to inserted value or the element which has same key.

**Complexity**
Amortized `O(M)` for insertion, `O(1)` for searching insertion point with valid `hint` otherwise `O(log(N))`.

**Invalidation**
Same as `Container::insert`.

```cpp
template <typename InputIterator>
void insert(InputIterator first, InputIterator last);

void insert(std::initializer_list<value_type> ilist);
```

Range insertion. Same effect as `insert(range_order::no_ordered, first, last)` and `insert(range_order::no_ordered, ilist)` respectively.

```cpp
insert_return_type insert(node_type&& node)

iterator insert(const_iterator hint, node_type&& node)
```

Insert `node`.
The `hint` is used in same way as other `insert()`.

**Return value**
An iterator to inserted value or the element which has same key.

**Complexity**
Amortized `O(M)` for insertion, `O(log(N))` for searching insertion point.

**Invalidation**
Same as `Container::insert`.

```cpp
template <typename InputIterator>
void insert(range_order order, InputIterator first, InputIterator last);

void insert(range_order order, std::initializer_list<value_type> ilist);
```

Range insertion with ordered or non-ordered range.

**Pre requirements**
`InputIterator` should meet [*InputIterator*](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator).
If the `order` is `range_order::sorted` or `range_order::unique_sorted`, the ranges should be sorted in `Compare` order, otherwise the behaviour is undefined.

**Complexity**
For non sorted range, amortized `O((N+E) log(N+E))` if enough additional memory is available, otherwise amortized `O((N+E) log(N+E)^2)`.
For sorted range, amortized `O(N+E)` if enough additional memory is available, otherwise amortized `O((N+E) log(N+E))`

**Invalidation**
Same as `Container::insert`.

### emplace

```cpp
template <typename... Args>
std::pair<iterator, bool> emplace(Args&&... args);
```
Equivalent to `insert(value_type(std::forward<Args>(args)...))`.

### emplace_hint

```cpp
template <typename... Args>
iterator emplace_hint(const_iterator hint, Args&&... args);
```

Equivalent to `insert(hint, value_type(std::forward<Args>(args)...))`.

### erase

```cpp
iterator erase(iterator pos);

iterator erase(const_iterator first, const_iterator last);

size_type erase(key_type const& key)
```

**Return value**
An iterator that next to erased elements.

**Complexity**
`O(M)`in first and second form.
`O(M)` for erasing, `O(log(N))` for searching target element in third form.

**Invalidation**
Same as `Container::erase`.

### swap

```cpp
void swap(flat_set& other) noexcept(/* see below */);
```

Swap elements, allocator, and comparator.

**Exceptions**
No except only if it meets all of
- `std::allocator_traits<allocator_type>::is_always_equal::value` and
- `std::is_nothrow_swappable<Compare>::value`

#### extract

```cpp
node_type extract(const_iterator position);

node_type extract(key_type const& key);
```

Extract an element and returns it.
Unlike `std::set::extract`, move element.

**Pre requirements**
`position` should be valid dereferenceable iterator in first form.

**Return value**
Extracted node handle.

**Complexity**
`O(M)` in first form.
`O(M)` for extraction, `O(log(N))` for searching target element in second form.

### merge

```cpp
template <typename Comp, typename Allocator>
void merge(std::set<key_type, Comp, Allocator>& source);

template <typename Comp, typename Allocator>
void merge(std::set<key_type, Comp, Allocator>&& source);

template <typename Comp, typename Allocator>
void merge(std::multiset<key_type, Comp, Allocator>& source);

template <typename Comp, typename Allocator>
void merge(std::multiset<key_type, Comp, Allocator>&& source);

template <typename Comp, typename Cont>
void merge(flat_set<key_type, Comp, Cont>& source);

template <typename Comp, typename Cont>
void merge(flat_set<key_type, Comp, Cont>&& source);

template <typename Comp, typename Cont>
void merge(flat_multiset<key_type, Comp, Cont>& source);

template <typename Comp, typename Cont>
void merge(flat_multiset<key_type, Comp, Cont>&& source);
```

Merge `source` container into self.

**Complexity**
Amortized `O(M E)` for insertion. `O(N+E)` for searching insertion point if `source` ordered in same order, otherwise `O(E log(N))`.

## Lookup

### count

```cpp
size_type count(key_type const& key) const;

template <typename K>
size_type count(K const& key) const;
```

Count number of elements.

The second form is only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(log(N))`.

### find

```cpp
iterator find(key_type const& key)

const_iterator find(key_type const& key) const;

template <typename K>
iterator find(K const& key);

template <typename K>
const_iterator find(K const& key) const;
```

Find an element.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(log(N))`.

### contains

```cpp
bool contains(key_type const& key) const;

template <typename K>
bool contains(K const& key) const;
```

The second form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(log(N))`.

### equal_range

```cpp
std::pair<iterator, iterator> equal_range(key_type const& key);

std::pair<const_iterator, const_iterator> equal_range(key_type const& key) const;

template <typename K>
std::pair<iterator, iterator> equal_range(K const& key);

template <typename K>
std::pair<const_iterator, const_iterator> equal_range(K const& key) const;
```

Returns a range that contains elements with specified key.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(log(N))`.

### lower_bound

```cpp
iterator lower_bound(key_type const& key);

const_iterator lower_bound(key_type const& key) const;

template <typename K>
iterator lower_bound(K const& key);

template <typename K>
const_iterator lower_bound(K const& key) const;
```

Returns an iterator that points to first element which is *not less* than specified kye.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(log(N))`.

### upper_bound

```cpp
iterator upper_bound(key_type const& key);

const_iterator upper_bound(key_type const& key) const;

template <typename K>
iterator upper_bound(K const& key);

template <typename K>
const_iterator upper_bound(K const& key) const;
```

Returns an iterator that points to first element which is *greater* than specified kye.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(log(N))`.

## Observers

```cpp
key_compare key_comp() const;

value_compare value_comp() const;
```

## Non member functions

### operator==

```cpp
template <typename Key, typename Compare, typename Container>
bool operator==(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Complexity**
`O(N)`.

### operator!=

```cpp
template <typename Key, typename Compare, typename Container>
bool operator!=(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Complexity**
`O(N)`.

**Standard**
This function is removed if
- `__cpp_impl_three_way_comparison` is defined, and
- `__cpp_lib_three_way_comparison` is defined.

### operator<

```cpp
template <typename Key, typename Compare, typename Container>
bool operator<(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Complexity**
`O(N)`.

**Standard**
This function is removed if
- `__cpp_impl_three_way_comparison` is defined, and
- `__cpp_lib_three_way_comparison` is defined.

### operator<=

```cpp
template <typename Key, typename Compare, typename Container>
bool operator<=(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Complexity**
`O(N)`.

**Standard**
This function is removed if
- `__cpp_impl_three_way_comparison` is defined, and
- `__cpp_lib_three_way_comparison` is defined.

### operator>

```cpp
template <typename Key, typename Compare, typename Container>
bool operator>(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Complexity**
`O(N)`.

**Standard**
This function is removed if
- `__cpp_impl_three_way_comparison` is defined, and
- `__cpp_lib_three_way_comparison` is defined.

### operator>=

```cpp
template <typename Key, typename Compare, typename Container>
bool operator>=(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Complexity**
`O(N)`.

**Standard**
This function is removed if
- `__cpp_impl_three_way_comparison` is defined, and
- `__cpp_lib_three_way_comparison` is defined.

### operator<=>

```cpp
template <typename Key, typename Compare, typename Container>
/* see below */ operator<=>(flat_set<Key, Compare, Container> const& lhs, flat_set<Key, Compare, Container> const& rhs);
```

**Return value**
`std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end())`

**Complexity**
`O(N)`.

**Standard**
This function is defined only if
- `__cpp_impl_three_way_comparison` is defined, and
- `__cpp_lib_three_way_comparison` is defined.

### swap

```cpp
template <typename Key, typename Compare, typename Container>
void swap(flat_set<Key, Compare, Container>& lhs, flat_set<Key, Compare, Container>& rhs) noexcept(/* see below */);
```

**Exceptions**
No except only if `noexcept(lhs.swap(rhs))` is true.

### erase_if

```cpp
template <typename Key, typename Compare, typename Container, typename Pred>
std::size_t erase_if(flat_set<Key, Compare, Container>& c, Pred pred);
```

Erase every elements which `pred` returned true.

**Return value**
Number of erased elements.

**Complexity**
`O(N)`.

## Deduction guides

```cpp
template <typename InputIterator,
          typename Compare = std::less<iter_val_t<InputIterator>>,
          typename Allocator = typename std::vector<iter_val_t<InputIterator>>::allocator_type>
flat_set(InputIterator, InputIterator, Compare = Compare(), Allocator = Allocator())
  -> flat_set<iter_val_t<InputIterator>, Compare, std::vector<iter_val_t<InputIterator>, Allocator>>;

template <typename Key,
          typename Compare = std::less<Key>,
          typename Allocator = typename std::vector<Key>::allocator_type>
flat_set(std::initializer_list<Key>, Compare = Compare(), Allocator = Allocator())
  -> flat_set<Key, Compare, std::vector<Key, Allocator>>;

template <typename InputIterator, typename Allocator>
flat_set(InputIterator, InputIterator, Allocator)
  -> flat_set<iter_val_t<InputIterator>, std::less<iter_val_t<InputIterator>>, std::vector<iter_val_t<InputIterator>, Allocator>>;

template <typename Key, typename Allocator>
flat_set(std::initializer_list<Key>, Allocator)
  -> flat_set<Key, std::less<Key>, std::vector<Key, Allocator>>;
```

Where the exposition only type aliases are defined as

```cpp
template <typename InputIterator>
using iter_val_t = typename std::iterator_traits<InputIterator>::value_type;
```

First and second form are participants in overload resolution only if `Compare` doesn't satisfy [*Allocator*](https://en.cppreference.com/w/cpp/named_req/Allocator), and `Allocator` satisfies [*Allocator*](https://en.cppreference.com/w/cpp/named_req/Allocator).
Third and fourth form are participants in overload resolution only if `Allocator` satisfies [*Allocator*](https://en.cppreference.com/w/cpp/named_req/Allocator).

