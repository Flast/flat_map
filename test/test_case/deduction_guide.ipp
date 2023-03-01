// Copyright (c) 2021,2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch_all.hpp>
#include <vector>
#include <memory>
#include <functional>

#include "config.hpp"

template <typename T>
struct stateful_allocator : public std::allocator<T>
{
    std::string state;

    // Hide std::allocator::rebind for C++17 mode.
    template <typename U>
    struct rebind { using other = stateful_allocator<U>; };

    stateful_allocator(char const* state) : state{state} {};
};

TEST_CASE("deduction guide", "[deduction guide]")
{
    std::vector const v =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("iter, iter, (comp), (alloc)")
    {
        {
            FLAT_CONTAINER_KEY fm(v.begin(), v.end());
            REQUIRE(fm.size() == 4);
        }
#if FLAT_MAP
        {
            FLAT_CONTAINER_KEY fm = {v.begin(), v.end()};
            REQUIRE(fm.size() == 4);
        }
#endif
    }

    SECTION("init_list, (comp), (alloc)")
    {
        {
            FLAT_CONTAINER_KEY fm{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)};
            REQUIRE(fm.size() == 4);
        }
        {
            FLAT_CONTAINER_KEY fm = {MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("iter, iter, comp, (alloc)")
    {
        {
            FLAT_CONTAINER_KEY fm{v.begin(), v.end(), std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
        {
            FLAT_CONTAINER_KEY fm = {v.begin(), v.end(), std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("init_list, comp, (alloc)")
    {
        {
            FLAT_CONTAINER_KEY fm{{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}, std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
        {
            FLAT_CONTAINER_KEY fm = {{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}, std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("iter, iter, alloc")
    {
        {
            FLAT_CONTAINER_KEY fm{v.begin(), v.end(), stateful_allocator<PAIR<int, int>>{"state"}};
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
        {
            FLAT_CONTAINER_KEY fm = {v.begin(), v.end(), stateful_allocator<PAIR<int, int>>{"state"}};
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
    }

    SECTION("init_list, alloc")
    {
        {
            FLAT_CONTAINER_KEY fm
            {
              {MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)},
              stateful_allocator<PAIR<int, int>>{"state"},
            };
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
        {
            FLAT_CONTAINER_KEY fm =
            {
              {MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)},
              stateful_allocator<PAIR<int, int>>{"state"},
            };
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
    }
}
