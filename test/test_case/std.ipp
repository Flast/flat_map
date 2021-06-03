// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>

#include "config.hpp"
#include STD_HEADER

TEST_CASE("merge", "[merge]")
{
    SECTION("from std container with same order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        STD_CONTAINER<int, int> m =
        {
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(7, 8),
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
        REQUIRE(m == STD_CONTAINER<int, int>{MAKE_PAIR(4, 9)});
    }

    SECTION("from std container with reversed order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        STD_CONTAINER<int, int, std::greater<int>> m =
        {
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(7, 8),
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
        REQUIRE(m == STD_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(4, 9)});
    }

    SECTION("from std multi container with same order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        STD_MULTI_CONTAINER<int, int> m =
        {
            MAKE_PAIR(1, 5),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
            MAKE_PAIR(7, 8),
            MAKE_PAIR(7, 2),
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
        REQUIRE(m == STD_MULTI_CONTAINER<int, int>{MAKE_PAIR(1, 2), MAKE_PAIR(4, 9), MAKE_PAIR(4, 8), MAKE_PAIR(7, 2)});
    }

    SECTION("from std multi container with reversed order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        STD_MULTI_CONTAINER<int, int, std::greater<int>> m =
        {
            MAKE_PAIR(1, 5),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
            MAKE_PAIR(7, 8),
            MAKE_PAIR(7, 2),
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
        REQUIRE(m == STD_MULTI_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(1, 2), MAKE_PAIR(4, 9), MAKE_PAIR(4, 8), MAKE_PAIR(7, 2)});
    }

    SECTION("from flat container with same order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_CONTAINER<int, int> m =
        {
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(7, 8),
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
        REQUIRE(m == FLAT_CONTAINER<int, int>{MAKE_PAIR(4, 9)});
    }

    SECTION("from flat container with reversed order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_CONTAINER<int, int, std::greater<int>> m =
        {
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(7, 8),
        };

        fm.merge(m);

        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
        REQUIRE(m == FLAT_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(4, 9)});
    }
}
