# flat\_map

```
template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Container = std::vector<std::pair<Key, T>>>
class flat_map;
```

**Requirements**
- `Container` should meets [*Container*](https://en.cppreference.com/w/cpp/named_req/SequenceContainer), [*AllocatorAwareContainer*](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer), [*SequenceContainer*](https://en.cppreference.com/w/cpp/named_req/SequenceContainer), and [*ReversibleContainer*](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer).

## Member types

```
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
using node_type = /* unspecified */;
using insert_return_type = struct /* unspecified */
{
    iterator  position;
    bool      inserted;
    node_type node;
};
```

## Member classes

```
struct value_compare;
```

### Constructor

```
protected:
value_type(Compare c);
```

### Operator

```
bool operator()(value_type const& lhs, value_type const& rhs);
```

**Return value**
`comp(lhs.first, rhs.first)`

## Constructors

```
flat_map();

explicit flat_map(Compare const& comp, allocator_type const& alloc = allocator_type());
```

```
template <typename InputIterator>
flat_map(InputIterator first, InputIterator last, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type());

template <typename InputIterator>
flat_map(InputIterator first, InputIterator last, allocator_type const& alloc);
```

Construct container from `[first, last)`.

```
flat_map(flat_map const& other);
``
flat_map(flat_map const& other, allocator_type const& alloc);
```

Copy from other.

```
flat_map(flat_map&& other);

flat_map(flat_map&& other, allocator_type const& alloc);
```

Move entire elements from other.

```
flat_map(std::initializer_list<value_type> init, Compare const& comp = Compare(), allocator_type const& alloc = allocator_type());

flat_map(std::initializer_list<value_type> init, allocator_type const& alloc);
```

Construct from init.

## Assignments

```
flat_map& operator=(flat_map const& other);
```

Copy from other.

```
flat_map& operator=(flat_map&& other) noexcept(/* see below */);
```

Move entire elements from other.

**Exceptions**
No except only if it meets all of
- `std::is_nothrow_move_assignable_v<Container> == true` and
- `std::is_nothrow_move_assignable_v<Compare> == true`

```
flat_map& operator=(std::initializer_list<value_type> ilist);
```

## Element access

```
mapped_type const& at(key_type const& key) const;
mapped_type& at(key_type const& key);
```

**Exceptions**
Throws `std::out_of_range` only if `key` is not found.

**Complexity**
`O(log N).

```
mapped_type& operator[](key_type const& key);
mapped_type& operator[](key_type&& key);
```

**Complexity**
`O(log N).

## Iterators

```
iterator begin() noexcept;
iterator begin() const noexcept;
const_iterator cbegin() const noexcept;
iterator end() noexcept;
iterator end() const noexcept;
const_iterator cend() const noexcept;
reverse_iterator rbegin() noexcept;
reverse_iterator rbegin() const noexcept;
const_reference crbegin() const noexcept;
reverse_iterator rend() noexcept;
reverse_iterator rend() const noexcept;
const_reference crend() const noexcept;
```

## Capacity

```
bool empty() const noexcept;
size_type size() const noexcept;
size_type max_size() const noexcept;
```

```
void reserve(size_type new_cap);
```

**Postcondition**
- `capacity() == new_cap`

```
size_type capacity() const noexcept;
```

```
void shrink_to_fit();
```

**Postcondition**
- `capacity() == size()`

## Modifiers

```
void clear();
```

Clear container.

**Postcondition**
- `size() == 0`

**Invalidation**
Invalidates every interators and references.

```
std::pair<iterator, bool> insert(value_type const& value);

template <typename V>
std::pair<iterator, bool> insert(V&& value);

std::pair<iterator, bool> insert(value_type&& value);
```

Insert `value`.

The second form only participants in overload resolution if `std::is_constructible_v<value_type, V&&> == true`.

**Return value**
An iterator to inserted value or the element which has same key, `bool` denotes whether the insertion is succeeded.

**Complexity**
Amortized `O(N)`.

**Invalidation**
Same as `Container::insert`.

```
iterator insert(const_iterator hint, value_type const&& value);

template <typename V>
iterator insert(const_iterator hint, V&& value);

iterator insert(const_iterator hint, value_type&& value);
```

Insert `value`.
The `hint` is used for looking up insertion position.

The second form only participants in overload resolution if `std::is_constructible_v<value_type, V&&> == true`.

**Return value**
An iterator to inserted value or the element which has same key.

**Complexity**
Amortized `O(N)`.

**Invalidation**
Same as `Container::insert`.

```
template <typename InputIterator>
void insert(InputIterator first, InputIterator last);

void insert(std::initializer_list<value_type> ilist);
```

Range insertion.

**Pre requirements**
`InputIterator` should meet *InputIterator*.

**Complexity**
Amortized `O(N logN)`.

**Invalidation**
Same as `Container::insert`.

```
insert_return_type insert(node_type&& node)

iterator insert(const_iterator hint, node_type&& node)
```

Insert `node`.
The `hint` is used in same way as other `insert()`.

**Return value**
An iterator to inserted value or the element which has same key.

**Complexity**
Amortized `O(N)`.

**Invalidation**
Same as `Container::insert`.

```
template <typename InputIterator>
void insert_sorted(InputIterator first, InputIterator last);

void insert_sorted(std::initializer_list<value_type> ilist);
```

Range insertion with sorted range.

**Pre requirements**
The ranges should be sorted in `Compare` order, otherwise the behaviour is undefined (Note that it doesn't require deduplication).

**Complexity**
Amortized `O(N^2)`.

**Invalidation**
Same as `Container::insert`.

```
template <typename M>
std::pair<iterator, bool> insert_or_assign(key_type const& key, M&& obj);

template <typename M>
std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj);

template <typename M>
iterator insert_or_assign(const_iterator hint, key_type const& key, M&& obj);

template <typename M>
iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj);
```

Same as `insert()` except replace with obj if key is always exists.

```
template <typename... Args>
std::pair<iterator, bool> emplace(Args&&... args);

template <typename... Args>
iterator emplace_hint(const_iterator hint, Args&&... args);
```

Equivalent to `insert(value_type(std::forward<Args>(args)...))`.

```
template <typename... Args>
std::pair<iterator, bool> try_emplace(key_type const& key, Args&&... args);

template <typename... Args>
std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args);

template <typename... Args>
iterator try_emplace(const_iterator hint, key_type const& key, Args&&... args);

template <typename... Args>
iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args);
```

Equivalent to `insert(value_type(key, std::forward<Args>(args)...))`.

```
iterator erase(iterator pos);

iterator erase(const_iterator first, const_iterator last);

size_type erase(key_type const& key)
```

Erase elements.

**Return value**
An iterator that next to erased elements.

**Complexity**
`O(N)`

```
void swap(flat_map& other) noexcept(/* see below */);
```

Swap elements, allocator, and comparator.

**Exceptions**
No except only if it meets all of
- `std::allocator_traits<allocator_type>::is_always_equal::value` and
- `std::is_nothrow_swappable<Compare>::value`

```
node_type extract(const_iterator position);

node_type extract(key_type const& key);
```

Extract an element and returns it.
Unlike `std::map::extract`, move element.

**Return value**
Extracted node handle.

**Complexity**
`O(N)`

```
template <typename Comp, typename Allocator>
void merge(std::map<key_type, mapped_type, Comp, Allocator>& source);

template <typename Comp, typename Allocator>
void merge(std::map<key_type, mapped_type, Comp, Allocator>&& source);

template <typename Comp, typename Allocator>
void merge(std::multimap<key_type, mapped_type, Comp, Allocator>& source);

template <typename Comp, typename Allocator>
void merge(std::multimap<key_type, mapped_type, Comp, Allocator>&& source);

template <typename Comp, typename Cont>
void merge(flat_map<key_type, mapped_type, Comp, Cont>& source);

template <typename Comp, typename Cont>
void merge(flat_map<key_type, mapped_type, Comp, Cont>&& source);
```

Merge `source` container into self.

**Complexity**
Amortized `O(N)` if comparators are same, otherwise amortized `O(N^2)`.

### Lookup

```
size_type count(key_type const& key) const;

template <typename K>
size_type count(K const& key) const;
```

Count number of elements.

The second form is only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(logN)`

```
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
`O(logN)`

```
bool contains(key_type const& key) const;

template <typename K>
bool contains(K const& key);
```

The second form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(logN)`

```
std::pair<iterator, iterator> equal_range(key_type const& key);

std::pair<const_iterator, const_iterator> equal_range(key_type const& key) const;

template <typename K>
enable_if_transparent<K, std::pair<iterator, iterator>> equal_range(K const& key);

template <typename K>
std::pair<const_iterator, const_iterator> equal_range(K const& key) const;
```

Returns a range that contains elements with specified key.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(logN)`

```
iterator lower_bound(key_type const& key);

const_iterator lower_bound(key_type const& key) const;

template <typename K>
enable_if_transparent<K, iterator> lower_bound(K const& key);

template <typename K>
const_iterator lower_bound(K const& key) const;
```

Returns an iterator that points to first element which is *not less* than specified kye.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(logN)`

```
iterator upper_bound(key_type const& key);

const_iterator upper_bound(key_type const& key) const;

template <typename K>
enable_if_transparent<K, iterator> upper_bound(K const& key);

template <typename K>
const_iterator upper_bound(K const& key) const;
```

Returns an iterator that points to first element which is *greater* than specified kye.

The third and fourth form are only participants in overload resolution if the `Compare::is_transparent` is valid.

**Complexity**
`O(logN)`

### Observers

```
key_compare key_comp() const;

value_compare value_comp() const;
```
