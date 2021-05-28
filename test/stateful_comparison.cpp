// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_map.hpp"

#include <catch2/catch.hpp>

bool less(int lhs, int rhs) { return lhs < rhs; }
bool less2(int lhs, int rhs) { return lhs < rhs; }
bool greater(int lhs, int rhs) { return lhs > rhs; }

TEST_CASE("construction fp", "[construction][fp]")
{
    SECTION("iter construction")
    {
        std::vector<std::pair<int, int>> v =
        {
            {6, 7},
            {4, 5},
            {2, 3},
            {0, 1},
        };
        flat_map::flat_map<int, int, bool(*)(int, int)> fm{v.begin(), v.end(), less};

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
        flat_map::flat_map<int, int, bool(*)(int, int)> fm =
        {
            {
                {6, 7},
                {4, 5},
                {2, 3},
                {0, 1},
            },
            less,
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
        flat_map::flat_map<int, int, bool(*)(int, int)> fm =
        {
            {
                {6, 7},
                {4, 5},
                {2, 3},
                {0, 1},
            },
            less,
        };
        auto* ptr = &*fm.begin();

        auto copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        copy.emplace(1, 2);

        auto itr = copy.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 1);
        REQUIRE(itr->second == 2);
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
        flat_map::flat_map<int, int, bool(*)(int, int)> fm =
        {
            {
                {6, 7},
                {4, 5},
                {2, 3},
                {0, 1},
            },
            less,
        };
        auto* ptr = &*fm.begin();

        auto move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        move.emplace(1, 2);

        auto itr = move.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 1);
        REQUIRE(itr->second == 2);
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

TEST_CASE("assignment", "[assignment][fp]")
{
    SECTION("copy assignment")
    {
        flat_map::flat_map<int, int, bool(*)(int, int)> fm =
        {
            {
                {6, 7},
                {4, 5},
                {2, 3},
                {0, 1},
            },
            less,
        };
        auto* ptr = &*fm.begin();

        flat_map::flat_map<int, int, bool(*)(int, int)> copy =
        {
            {
                {1, 1},
            },
            greater,
        };
        copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        copy.emplace(1, 2);

        auto itr = copy.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 1);
        REQUIRE(itr->second == 2);
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
        flat_map::flat_map<int, int, bool(*)(int, int)> fm =
        {
            {
                {6, 7},
                {4, 5},
                {2, 3},
                {0, 1},
            },
            less,
        };
        auto* ptr = &*fm.begin();

        flat_map::flat_map<int, int, bool(*)(int, int)> move =
        {
            {
                {1, 1},
            },
            greater,
        };
        move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        move.emplace(1, 2);

        auto itr = move.begin();
        REQUIRE(itr->first == 0);
        REQUIRE(itr->second == 1);
        ++itr;
        REQUIRE(itr->first == 1);
        REQUIRE(itr->second == 2);
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

TEST_CASE("swap", "[swap][fp]")
{
    SECTION("swap comparator")
    {
        flat_map::flat_map<int, int, bool(*)(int, int)> fm =
        {
            {
                {0, 1},
                {2, 3},
                {4, 5},
                {6, 7},
            },
            less,
        };

        flat_map::flat_map<int, int, bool(*)(int, int)> dst =
        {
            {
                {1, 2},
                {3, 4},
            },
            greater,
        };

        fm.swap(dst);

        fm.emplace(2, 3);

        auto itr = fm.begin();
        REQUIRE(itr->first == 3);
        REQUIRE(itr->second == 4);
        ++itr;
        REQUIRE(itr->first == 2);
        REQUIRE(itr->second == 3);
        ++itr;
        REQUIRE(itr->first == 1);
        REQUIRE(itr->second == 2);
        ++itr;
        REQUIRE(itr == fm.end());
    }
}
