// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_set.hpp"

#include <catch2/catch.hpp>

bool less(int lhs, int rhs) { return lhs < rhs; }
bool less2(int lhs, int rhs) { return lhs < rhs; }
bool greater(int lhs, int rhs) { return lhs > rhs; }

TEST_CASE("construction fp", "[construction][fp]")
{
    SECTION("iter construction")
    {
        std::vector v =
        {
            6,
            4,
            2,
            0,
        };
        flat_map::flat_set<int, bool(*)(int, int)> fm{v.begin(), v.end(), less};

        auto itr = fm.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fm.end());
    }

    SECTION("init list construction")
    {
        flat_map::flat_set<int, bool(*)(int, int)> fm =
        {
            {
                6,
                4,
                2,
                0,
            },
            less,
        };

        auto itr = fm.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fm.end());
    }

    SECTION("copy construction")
    {
        flat_map::flat_set<int, bool(*)(int, int)> fm =
        {
            {
                6,
                4,
                2,
                0,
            },
            less,
        };
        auto* ptr = &*fm.begin();

        auto copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        copy.emplace(1);

        auto itr = copy.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == copy.end());
    }

    SECTION("move construction")
    {
        flat_map::flat_set<int, bool(*)(int, int)> fm =
        {
            {
                6,
                4,
                2,
                0,
            },
            less,
        };
        auto* ptr = &*fm.begin();

        auto move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        move.emplace(1);

        auto itr = move.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("assignment", "[assignment][fp]")
{
    SECTION("copy assignment")
    {
        flat_map::flat_set<int, bool(*)(int, int)> fm =
        {
            {
                6,
                4,
                2,
                0,
            },
            less,
        };
        auto* ptr = &*fm.begin();

        flat_map::flat_set<int, bool(*)(int, int)> copy =
        {
            {
                1,
            },
            greater,
        };
        copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        copy.emplace(1);

        auto itr = copy.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == copy.end());
    }

    SECTION("move assignment")
    {
        flat_map::flat_set<int, bool(*)(int, int)> fm =
        {
            {
                6,
                4,
                2,
                0,
            },
            less,
        };
        auto* ptr = &*fm.begin();

        flat_map::flat_set<int, bool(*)(int, int)> move =
        {
            {
                1,
            },
            greater,
        };
        move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        move.emplace(1);

        auto itr = move.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("swap", "[swap][fp]")
{
    SECTION("swap comparator")
    {
        flat_map::flat_set<int, bool(*)(int, int)> fm =
        {
            {
                0,
                2,
                4,
                6,
            },
            less,
        };

        flat_map::flat_set<int, bool(*)(int, int)> dst =
        {
            {
                1,
                3,
            },
            greater,
        };

        fm.swap(dst);

        fm.emplace(2);

        auto itr = fm.begin();
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(itr == fm.end());
    }
}
