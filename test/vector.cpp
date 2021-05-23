// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_map.hpp"

#include <vector>
#include <memory>

template <typename T>
using CONTAINER = std::vector<T>;

#include "basic.ipp"

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
    std::vector<std::pair<int, int>> const v =
    {
        {0, 1},
        {2, 3},
        {4, 5},
        {6, 7},
    };

    SECTION("iter, iter, (comp), (alloc)")
    {
        {
            flat_map::flat_map fm{v.begin(), v.end()};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_map fm = {v.begin(), v.end()};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("init_list, (comp), (alloc)")
    {
        {
            flat_map::flat_map fm{std::pair{0, 1}, std::pair{2, 3}, std::pair{4, 5}, std::pair{6, 7}};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_map fm = {std::pair{0, 1}, std::pair{2, 3}, std::pair{4, 5}, std::pair{6, 7}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("iter, iter, comp, (alloc)")
    {
        {
            flat_map::flat_map fm{v.begin(), v.end(), std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_map fm = {v.begin(), v.end(), std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("init_list, comp, (alloc)")
    {
        {
            flat_map::flat_map fm{{std::pair{0, 1}, std::pair{2, 3}, std::pair{4, 5}, std::pair{6, 7}}, std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_map fm = {{std::pair{0, 1}, std::pair{2, 3}, std::pair{4, 5}, std::pair{6, 7}}, std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("iter, iter, alloc")
    {
        {
            flat_map::flat_map fm{v.begin(), v.end(), stateful_allocator<std::pair<int, int>>{"state"}};
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
        {
            flat_map::flat_map fm = {v.begin(), v.end(), stateful_allocator<std::pair<int, int>>{"state"}};
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
    }

    SECTION("init_list, alloc")
    {
        {
            flat_map::flat_map fm
            {
              {std::pair{0, 1}, std::pair{2, 3}, std::pair{4, 5}, std::pair{6, 7}},
              stateful_allocator<std::pair<int, int>>{"state"},
            };
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
        {
            flat_map::flat_map fm =
            {
              {std::pair{0, 1}, std::pair{2, 3}, std::pair{4, 5}, std::pair{6, 7}},
              stateful_allocator<std::pair<int, int>>{"state"},
            };
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
    }
}
