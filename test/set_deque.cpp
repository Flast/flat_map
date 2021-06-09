// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_set.hpp"

#include <deque>

template <typename T>
using CONTAINER = std::deque<T>;

#define FLAT_MAP 0
#define MULTI_CONTAINER 0
#include "test_case/basic.ipp"
#include "test_case/stateful_comparison.ipp"
#include "test_case/std.ipp"
