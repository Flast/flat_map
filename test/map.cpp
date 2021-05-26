// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_map.hpp"

#include <catch2/catch.hpp>
#include <map>

TEST_CASE("merge", "[merge]")
{
    SECTION("from map with same order")
    {
        flat_map::flat_map<int, int> fm =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        std::map<int, int> m =
        {
            {1, 2},
            {4, 9},
            {7, 8},
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        flat_map::flat_map<int, int> ans =
        {
            {0, 1},
            {1, 2},
            {2, 3},
            {4, 5},
            {6, 7},
            {7, 8},
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::map<int, int>{{4, 9}});
    }

    SECTION("from map with reversed order")
    {
        flat_map::flat_map<int, int> fm =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        std::map<int, int, std::greater<int>> m =
        {
            {1, 2},
            {4, 9},
            {7, 8},
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        flat_map::flat_map<int, int> ans =
        {
            {0, 1},
            {1, 2},
            {2, 3},
            {4, 5},
            {6, 7},
            {7, 8},
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::map<int, int, std::greater<int>>{{4, 9}});
    }

    SECTION("from multimap with same order")
    {
        flat_map::flat_map<int, int> fm =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        std::multimap<int, int> m =
        {
            {1, 5},
            {1, 2},
            {4, 9},
            {4, 8},
            {7, 8},
            {7, 2},
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);

        flat_map::flat_map<int, int> ans =
        {
            {0, 1},
            {1, 5},
            {2, 3},
            {4, 5},
            {6, 7},
            {7, 8},
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::multimap<int, int>{{1, 2}, {4, 9}, {4, 8}, {7, 2}});
    }

    SECTION("from multimap with reversed order")
    {
        flat_map::flat_map<int, int> fm =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        std::multimap<int, int, std::greater<int>> m =
        {
            {1, 5},
            {1, 2},
            {4, 9},
            {4, 8},
            {7, 8},
            {7, 2},
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);

        flat_map::flat_map<int, int> ans =
        {
            {0, 1},
            {1, 5},
            {2, 3},
            {4, 5},
            {6, 7},
            {7, 8},
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::multimap<int, int, std::greater<int>>{{1, 2}, {4, 9}, {4, 8}, {7, 2}});
    }
}
