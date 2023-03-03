// Copyright (c) 2021,2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_multimap.hpp"
#include "flat_map/tied_sequence.hpp"

#include <vector>

template <typename T>
using CONTAINER = flat_map::tied_sequence<std::vector<std::tuple_element_t<0, T>>, std::vector<std::tuple_element_t<1, T>>>;

#define FLAT_MAP 1
#define MULTI_CONTAINER 1
#include "test_case/catch2_tuple.hpp"
#include "test_case/basic.ipp"
#include "test_case/stateful_comparison.ipp"
#include "test_case/std.ipp"
