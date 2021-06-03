// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <functional>

#include "flat_map/__fwd.hpp"

#if FLAT_MAP

#include <utility>

#define FLAT_CONTAINER_KEY flat_map::flat_map

template <typename T1, typename T2>
using PAIR = std::pair<T1, T2>;

template <typename T1, typename T2>
auto MAKE_PAIR(T1 v1, T2 v2) { return std::pair<T1, T2>{v1, v2}; }

#define PAIR_PARAM(v1, v2) v1, v2

template <typename KVP>
auto FIRST(KVP const& kvp) { return kvp.first; }

#define STD_HEADER <map>

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_CONTAINER = std::map<Key, T, Compare>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_MULTI_CONTAINER = std::multimap<Key, T, Compare>;

#else // FLAT_MAP

#define FLAT_CONTAINER_KEY flat_map::flat_set

template <typename T1, typename>
using PAIR = T1;

template <typename T1, typename T2>
auto MAKE_PAIR(T1 v1, T2) { return v1; }

#define PAIR_PARAM(v1, v2) v1

template <typename KVP>
auto FIRST(KVP const& kvp) { return kvp; }

#define STD_HEADER <set>

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_CONTAINER = std::set<Key, Compare>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using STD_MULTI_CONTAINER = std::multiset<Key, Compare>;

#endif // FLAT_MAP

template <typename Key, typename T, typename Compare = std::less<Key>>
using FLAT_CONTAINER = FLAT_CONTAINER_KEY<PAIR_PARAM(Key, T), Compare, CONTAINER<PAIR<Key, T>>>;
