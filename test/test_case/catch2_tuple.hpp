// Copyright (c) 2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#include "flat_map/__config.hpp"

#ifdef FLAT_MAP_ZIP_NON_STD_TUPLE

#include <catch2/catch_tostring.hpp>

#include "flat_map/__tuple.hpp"

namespace Catch
{
    template <typename... T>
    struct StringMaker<flat_map::detail::tuple<T...>> : StringMaker<std::tuple<T...>> { };
}

#endif
