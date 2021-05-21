// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_map.hpp"

#include <catch2/catch.hpp>

TEST_CASE("construction", "[construction]")
{
    SECTION("default construction")
    {
        flat_map::flat_map<int, int> fm;
    }

    SECTION("iter construction")
    {
        std::vector<std::pair<int, int>> v =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };
        flat_map::flat_map<int, int> fm{v.begin(), v.end()};

        auto itr = fm.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 4);
        REQUIRE(itr->second == 5);
        ++itr;
        REQUIRE(itr->first == 6);
        REQUIRE(itr->second == 7);
        ++itr;
        REQUIRE(itr == fm.end());
    }

    SECTION("init list construction")
    {
        flat_map::flat_map<int, int> fm =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };

        auto itr = fm.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 4);
        REQUIRE(itr->second == 5);
        ++itr;
        REQUIRE(itr->first == 6);
        REQUIRE(itr->second == 7);
        ++itr;
        REQUIRE(itr == fm.end());
    }

    SECTION("copy construction")
    {
        flat_map::flat_map<int, int> fm =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };
        auto* ptr = &*fm.begin();

        auto copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        auto itr = copy.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 4);
        REQUIRE(itr->second == 5);
        ++itr;
        REQUIRE(itr->first == 6);
        REQUIRE(itr->second == 7);
        ++itr;
        REQUIRE(itr == copy.end());
    }

    SECTION("move construction")
    {
        flat_map::flat_map<int, int> fm =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };
        auto* ptr = &*fm.begin();

        auto move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        auto itr = move.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 4);
        REQUIRE(itr->second == 5);
        ++itr;
        REQUIRE(itr->first == 6);
        REQUIRE(itr->second == 7);
        ++itr;
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("assignment", "[assignment]")
{
    SECTION("copy assignment")
    {
        flat_map::flat_map<int, int> fm =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };
        auto* ptr = &*fm.begin();

        flat_map::flat_map<int, int> copy =
        {
            {1, 1},
        };
        copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        auto itr = copy.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 4);
        REQUIRE(itr->second == 5);
        ++itr;
        REQUIRE(itr->first == 6);
        REQUIRE(itr->second == 7);
        ++itr;
        REQUIRE(itr == copy.end());
    }

    SECTION("move assignment")
    {
        flat_map::flat_map<int, int> fm =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };
        auto* ptr = &*fm.begin();

        flat_map::flat_map<int, int> move =
        {
            {1, 1},
        };
        move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        auto itr = move.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 4);
        REQUIRE(itr->second == 5);
        ++itr;
        REQUIRE(itr->first == 6);
        REQUIRE(itr->second == 7);
        ++itr;
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("size", "[size]")
{
    SECTION("empty")
    {
        flat_map::flat_map<int, int> fm;

        REQUIRE(fm.empty());
        REQUIRE(fm.size() == 0);
    }

    SECTION("non empty")
    {
        flat_map::flat_map<int, int> fm =
        {
            {1, 1},
        };

        REQUIRE_FALSE(fm.empty());
        REQUIRE(fm.size() == 1);
    }

    SECTION("clear")
    {
        flat_map::flat_map<int, int> fm =
        {
            {1, 1},
        };

        fm.clear();
        REQUIRE(fm.empty());
    }
}
