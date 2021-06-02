// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#pragma once

#define FLAT_MAP_COMPILER_VERSION(major, minor, patch) (((major * 1000) + minor) * 100 + patch)

#if defined(__GNUC__) && !defined(__clang__)
#   define FLAT_MAP_COMPILER_GCC FLAT_MAP_COMPILER_VERSION(__GNUC__, __GNU_MINOR__, __GNU_PATCHLEVEL__)
#else
#   define FLAT_MAP_COMPILER_GCC 0
#endif
