// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#if __has_include(<compare>)
#include <compare>
#endif

#define FLAT_MAP_COMPILER_VERSION(major, minor, patch) (((major * 1000) + minor) * 100 + patch)

#if defined(__GNUC__) && !defined(__clang__)
#   define FLAT_MAP_COMPILER_GCC FLAT_MAP_COMPILER_VERSION(__GNUC__, __GNU_MINOR__, __GNU_PATCHLEVEL__)
#else
#   define FLAT_MAP_COMPILER_GCC_DISABLED 1
#endif

#define FLAT_MAP_WORKAROUND(key, cond) ((!key ## _DISABLED) || (key cond))

#if defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_three_way_comparison)
#   define FLAT_MAP_HAS_THREE_WAY_COMPARISON 1
#endif
