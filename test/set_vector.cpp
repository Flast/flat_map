// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_set.hpp"

#include <vector>
#include <memory>

template <typename T>
using CONTAINER = std::vector<T>;

#include "set_basic.ipp"

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
        0,
        2,
        4,
        6,
    };

    SECTION("iter, iter, (comp), (alloc)")
    {
        {
            flat_map::flat_set fm(v.begin(), v.end());
            REQUIRE(fm.size() == 4);
        }
        // Compiler deduces flat_set<InputIterator>::flat_set(std::initializer_list<InputIterator>)
        // {
        //     flat_map::flat_set fm = {v.begin(), v.end()};
        //     REQUIRE(fm.size() == 4);
        // }
    }

    SECTION("init_list, (comp), (alloc)")
    {
        {
            flat_map::flat_set fm{0, 2, 4, 6};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_set fm = {0, 2, 4, 6};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("iter, iter, comp, (alloc)")
    {
        {
            flat_map::flat_set fm{v.begin(), v.end(), std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_set fm = {v.begin(), v.end(), std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("init_list, comp, (alloc)")
    {
        {
            flat_map::flat_set fm{{0, 2, 4, 6}, std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
        {
            flat_map::flat_set fm = {{0, 2, 4, 6}, std::greater<int>{}};
            REQUIRE(fm.size() == 4);
        }
    }

    SECTION("iter, iter, alloc")
    {
        {
            flat_map::flat_set fm{v.begin(), v.end(), stateful_allocator<int>{"state"}};
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
        {
            flat_map::flat_set fm = {v.begin(), v.end(), stateful_allocator<int>{"state"}};
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
    }

    SECTION("init_list, alloc")
    {
        {
            flat_map::flat_set fm
            {
              {0, 2, 4, 6},
              stateful_allocator<int>{"state"},
            };
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
        {
            flat_map::flat_set fm =
            {
              {0, 2, 4, 6},
              stateful_allocator<int>{"state"},
            };
            REQUIRE(fm.size() == 4);
            REQUIRE(fm.get_allocator().state == "state");
        }
    }
}
