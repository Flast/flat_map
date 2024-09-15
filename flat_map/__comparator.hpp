// Copyright (c) 2024 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include <type_traits>
#include <utility>

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

} // namespace flat_map::detail
