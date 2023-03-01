// Copyright (c) 2021,2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <functional>

#include "flat_map/__fwd.hpp"

#if FLAT_MAP

#include <map>
#include <utility>

#include "flat_map/flat_map.hpp"
#include "flat_map/flat_multimap.hpp"

#define FLAT_UNIQ_CONTAINER_KEY flat_map::flat_map
#define FLAT_MULTI_CONTAINER_KEY flat_map::flat_multimap

template <typename T1, typename T2>
using PAIR = std::pair<T1, T2>;

#define PAIR_PARAM(v1, v2) v1, v2

template <typename KVP>
auto FIRST(KVP const& kvp) { return std::get<0>(kvp); }

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_CONTAINER = std::map<Key, T, Compare>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_MULTI_CONTAINER = std::multimap<Key, T, Compare>;

#else // FLAT_MAP

#include <set>

#include "flat_map/flat_set.hpp"
#include "flat_map/flat_multiset.hpp"

#define FLAT_UNIQ_CONTAINER_KEY flat_map::flat_set
#define FLAT_MULTI_CONTAINER_KEY flat_map::flat_multiset

template <typename T1, typename>
using PAIR = T1;

#define PAIR_PARAM(v1, v2) v1

template <typename KVP>
auto FIRST(KVP const& kvp) { return kvp; }

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_CONTAINER = std::set<Key, Compare>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_MULTI_CONTAINER = std::multiset<Key, Compare>;

#endif // FLAT_MAP

#if MULTI_CONTAINER

#define INSERT_ITR(result) result
#define NODE_INSERT_ITR(result) result
#define REQUIRE_INSERTED(result)
#define REQUIRE_INSERTED_FALSE(result)
#define REQUIRE_NODE_INSERTED(result)
#define REQUIRE_NODE_INSERTED_FALSE(result)

#else

#define INSERT_ITR(result) result.first
#define NODE_INSERT_ITR(result) result.position
#define REQUIRE_INSERTED(result) REQUIRE(result.second)
#define REQUIRE_INSERTED_FALSE(result) REQUIRE_FALSE(result.second)
#define REQUIRE_NODE_INSERTED(result) REQUIRE(result.inserted)
#define REQUIRE_NODE_INSERTED_FALSE(result) REQUIRE_FALSE(result.inserted)

#endif

#if MULTI_CONTAINER
#   define FLAT_CONTAINER_KEY FLAT_MULTI_CONTAINER_KEY
#else
#   define FLAT_CONTAINER_KEY FLAT_UNIQ_CONTAINER_KEY
#endif

template <typename Key, typename T, typename Compare = std::less<Key>>
using FLAT_CONTAINER = FLAT_CONTAINER_KEY<PAIR_PARAM(Key, T), Compare, CONTAINER<PAIR<Key, T>>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using FLAT_UNIQ_CONTAINER = FLAT_UNIQ_CONTAINER_KEY<PAIR_PARAM(Key, T), Compare, CONTAINER<PAIR<Key, T>>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using FLAT_MULTI_CONTAINER = FLAT_MULTI_CONTAINER_KEY<PAIR_PARAM(Key, T), Compare, CONTAINER<PAIR<Key, T>>>;

template <typename T1, typename T2>
auto MAKE_PAIR(T1 t1, [[maybe_unused]] T2 t2)
{
    return typename CONTAINER<PAIR<T1, T2>>::value_type{PAIR_PARAM(t1, t2)};
}
