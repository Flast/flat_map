// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

namespace flat_map
{

enum class range_order
{
    no_ordered,
    sorted,
    uniqued,
    unique_sorted,
};

template <range_order order>
using range_order_t = std::integral_constant<range_order, order>;

} // namespace flat_map
