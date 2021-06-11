// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>
#include <vector>

#include "config.hpp"

bool less(int lhs, int rhs) { return lhs < rhs; }
bool less2(int lhs, int rhs) { return lhs < rhs; }
bool greater(int lhs, int rhs) { return lhs > rhs; }

TEST_CASE("construction fp", "[construction][fp]")
{
    SECTION("iter construction")
    {
        std::vector v =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(0, 1),
        };
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm{v.begin(), v.end(), less};

        auto itr = fm.begin();
        REQUIRE(*itr == MAKE_PAIR(0, 1));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(4, 5));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(6, 7));
        ++itr;
        REQUIRE(itr == fm.end());
    }

    SECTION("init list construction")
    {
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm =
        {
            {
                MAKE_PAIR(6, 7),
                MAKE_PAIR(4, 5),
                MAKE_PAIR(2, 3),
                MAKE_PAIR(0, 1),
            },
            less,
        };

        auto itr = fm.begin();
        REQUIRE(*itr == MAKE_PAIR(0, 1));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(4, 5));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(6, 7));
        ++itr;
        REQUIRE(itr == fm.end());
    }

    SECTION("copy construction")
    {
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm =
        {
            {
                MAKE_PAIR(6, 7),
                MAKE_PAIR(4, 5),
                MAKE_PAIR(2, 3),
                MAKE_PAIR(0, 1),
            },
            less,
        };
        auto* ptr = &*fm.begin();

        auto copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        copy.emplace(PAIR_PARAM(1, 2));

        auto itr = copy.begin();
        REQUIRE(*itr == MAKE_PAIR(0, 1));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(1, 2));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(4, 5));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(6, 7));
        ++itr;
        REQUIRE(itr == copy.end());
    }

    SECTION("move construction")
    {
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm =
        {
            {
                MAKE_PAIR(6, 7),
                MAKE_PAIR(4, 5),
                MAKE_PAIR(2, 3),
                MAKE_PAIR(0, 1),
            },
            less,
        };
        auto* ptr = &*fm.begin();

        auto move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        move.emplace(PAIR_PARAM(1, 2));

        auto itr = move.begin();
        REQUIRE(*itr == MAKE_PAIR(0, 1));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(1, 2));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(4, 5));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(6, 7));
        ++itr;
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("assignment fp", "[assignment][fp]")
{
    SECTION("copy assignment")
    {
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm =
        {
            {
                MAKE_PAIR(6, 7),
                MAKE_PAIR(4, 5),
                MAKE_PAIR(2, 3),
                MAKE_PAIR(0, 1),
            },
            less,
        };
        auto* ptr = &*fm.begin();

        FLAT_CONTAINER<int, int, bool(*)(int, int)> copy =
        {
            {
                MAKE_PAIR(1, 1),
            },
            greater,
        };
        copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        copy.emplace(PAIR_PARAM(1, 2));

        auto itr = copy.begin();
        REQUIRE(*itr == MAKE_PAIR(0, 1));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(1, 2));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(4, 5));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(6, 7));
        ++itr;
        REQUIRE(itr == copy.end());
    }

    SECTION("move assignment")
    {
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm =
        {
            {
                MAKE_PAIR(6, 7),
                MAKE_PAIR(4, 5),
                MAKE_PAIR(2, 3),
                MAKE_PAIR(0, 1),
            },
            less,
        };
        auto* ptr = &*fm.begin();

        FLAT_CONTAINER<int, int, bool(*)(int, int)> move =
        {
            {
                MAKE_PAIR(1, 1),
            },
            greater,
        };
        move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        move.emplace(PAIR_PARAM(1, 2));

        auto itr = move.begin();
        REQUIRE(*itr == MAKE_PAIR(0, 1));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(1, 2));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(4, 5));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(6, 7));
        ++itr;
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("swap fp", "[swap][fp]")
{
    SECTION("swap comparator")
    {
        FLAT_CONTAINER<int, int, bool(*)(int, int)> fm =
        {
            {
                MAKE_PAIR(0, 1),
                MAKE_PAIR(2, 3),
                MAKE_PAIR(4, 5),
                MAKE_PAIR(6, 7),
            },
            less,
        };

        FLAT_CONTAINER<int, int, bool(*)(int, int)> dst =
        {
            {
                MAKE_PAIR(1, 2),
                MAKE_PAIR(3, 4),
            },
            greater,
        };

        fm.swap(dst);

        fm.emplace(PAIR_PARAM(2, 3));

        auto itr = fm.begin();
        REQUIRE(*itr == MAKE_PAIR(3, 4));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(2, 3));
        ++itr;
        REQUIRE(*itr == MAKE_PAIR(1, 2));
        ++itr;
        REQUIRE(itr == fm.end());
    }
}
