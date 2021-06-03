// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_set.hpp"

#include <catch2/catch.hpp>
#include <set>

TEST_CASE("merge", "[merge]")
{
    SECTION("from map with same order")
    {
        flat_map::flat_set<int> fm =
        {
            0,
            2,
            4,
            6,
        };

        std::set<int> m =
        {
            1,
            4,
            7,
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        flat_map::flat_set<int> ans =
        {
            0,
            1,
            2,
            4,
            6,
            7,
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::set<int>{4});
    }

    SECTION("from map with reversed order")
    {
        flat_map::flat_set<int> fm =
        {
            0,
            2,
            4,
            6,
        };

        std::set<int, std::greater<int>> m =
        {
            1,
            4,
            7,
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        flat_map::flat_set<int> ans =
        {
            0,
            1,
            2,
            4,
            6,
            7,
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::set<int, std::greater<int>>{4});
    }

    SECTION("from multimap with same order")
    {
        flat_map::flat_set<int> fm =
        {
            0,
            2,
            4,
            6,
        };

        std::multiset<int> m =
        {
            1,
            1,
            4,
            4,
            7,
            7,
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);

        flat_map::flat_set<int> ans =
        {
            0,
            1,
            2,
            4,
            6,
            7,
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::multiset<int>{1, 4, 4, 7});
    }

    SECTION("from multimap with reversed order")
    {
        flat_map::flat_set<int> fm =
        {
            0,
            2,
            4,
            6,
        };

        std::multiset<int, std::greater<int>> m =
        {
            1,
            1,
            4,
            4,
            7,
            7,
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);

        flat_map::flat_set<int> ans =
        {
            0,
            1,
            2,
            4,
            6,
            7,
        };

        REQUIRE(fm == ans);
        REQUIRE(m == std::multiset<int, std::greater<int>>{1, 4, 4, 7});
    }

    SECTION("from flat_map with same order")
    {
        flat_map::flat_set<int> fm =
        {
            0,
            2,
            4,
            6,
        };

        flat_map::flat_set<int> m =
        {
            1,
            4,
            7,
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        flat_map::flat_set<int> ans =
        {
            0,
            1,
            2,
            4,
            6,
            7,
        };

        REQUIRE(fm == ans);
        REQUIRE(m == flat_map::flat_set<int>{4});
    }

    SECTION("from flat_map with reversed order")
    {
        flat_map::flat_set<int> fm =
        {
            0,
            2,
            4,
            6,
        };

        flat_map::flat_set<int, std::greater<int>> m =
        {
            1,
            4,
            7,
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        flat_map::flat_set<int> ans =
        {
            0,
            1,
            2,
            4,
            6,
            7,
        };

        REQUIRE(fm == ans);
        REQUIRE(m == flat_map::flat_set<int, std::greater<int>>{4});
    }
}
