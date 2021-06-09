// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>

#include "config.hpp"
#include STD_HEADER
#if FLAT_MAP
#include "flat_map/flat_map.hpp"
#include "flat_map/flat_multimap.hpp"
#else
#include "flat_map/flat_set.hpp"
#include "flat_map/flat_multiset.hpp"
#endif

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

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 7);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == STD_CONTAINER<int, int>{MAKE_PAIR(4, 9)});
#endif
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

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 7);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == STD_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(4, 9)});
#endif
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

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 10);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(1, 2),
#endif
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
#if MULTI_CONTAINER
            MAKE_PAIR(7, 2),
#endif
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == STD_MULTI_CONTAINER<int, int>{MAKE_PAIR(1, 2), MAKE_PAIR(4, 9), MAKE_PAIR(4, 8), MAKE_PAIR(7, 2)});
#endif
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

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 10);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(1, 2),
#endif
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
#if MULTI_CONTAINER
            MAKE_PAIR(7, 2),
#endif
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == STD_MULTI_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(1, 2), MAKE_PAIR(4, 9), MAKE_PAIR(4, 8), MAKE_PAIR(7, 2)});
#endif
    }

    SECTION("from flat unique container with same order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_UNIQ_CONTAINER<int, int> m =
        {
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(7, 8),
        };

        fm.merge(m);

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 7);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == FLAT_UNIQ_CONTAINER<int, int>{MAKE_PAIR(4, 9)});
#endif
    }

    SECTION("from flat unique container with reversed order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_UNIQ_CONTAINER<int, int, std::greater<int>> m =
        {
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(7, 8),
        };

        fm.merge(m);

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 7);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 1);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == FLAT_UNIQ_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(4, 9)});
#endif
    }

    SECTION("from flat multi container with same order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_MULTI_CONTAINER<int, int> m =
        {
            MAKE_PAIR(1, 5),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
            MAKE_PAIR(7, 8),
            MAKE_PAIR(7, 2),
        };

        fm.merge(m);

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 10);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(1, 2),
#endif
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
#if MULTI_CONTAINER
            MAKE_PAIR(7, 2),
#endif
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == FLAT_MULTI_CONTAINER<int, int>{MAKE_PAIR(1, 2), MAKE_PAIR(4, 9), MAKE_PAIR(4, 8), MAKE_PAIR(7, 2)});
#endif
    }

    SECTION("from flat multi container with reversed order")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_MULTI_CONTAINER<int, int, std::greater<int>> m =
        {
            MAKE_PAIR(1, 5),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
            MAKE_PAIR(7, 8),
            MAKE_PAIR(7, 2),
        };

        fm.merge(m);

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 10);
        REQUIRE(m.empty());
#else
        REQUIRE(fm.size() == 6);
        REQUIRE(m.size() == 4);
#endif

        FLAT_CONTAINER<int, int> ans =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(1, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(1, 2),
#endif
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
#if MULTI_CONTAINER
            MAKE_PAIR(4, 9),
            MAKE_PAIR(4, 8),
#endif
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
#if MULTI_CONTAINER
            MAKE_PAIR(7, 2),
#endif
        };

        REQUIRE(fm == ans);
#if !MULTI_CONTAINER
        REQUIRE(m == FLAT_MULTI_CONTAINER<int, int, std::greater<int>>{MAKE_PAIR(1, 2), MAKE_PAIR(4, 9), MAKE_PAIR(4, 8), MAKE_PAIR(7, 2)});
#endif
    }
}
