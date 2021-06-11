// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>
#include <iterator>
#include <stdexcept>

#include "config.hpp"

struct Value
{
    int value = 0xcccccccc;
    char const* name = "defaulted";

    Value() = default;
    explicit Value(int value, char const* name) : value{value}, name{name} {}
};

TEST_CASE("accessor", "[accessor]")
{
    FLAT_CONTAINER<int, int> fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("found on at")
    {
        auto& value = fm.at(2);
        REQUIRE(value == 3);
    }

    SECTION("not found on at")
    {
        REQUIRE_THROWS_AS(fm.at(3), std::out_of_range);
    }

    SECTION("found on op[]")
    {
        auto& value = fm[2];
        REQUIRE(value == 3);
    }

    SECTION("not found on op[]")
    {
        auto& value = fm[3];
        REQUIRE(fm.size() == 5);
        REQUIRE(value == int{});

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, int{}));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("insert or assign", "[insertion]")
{
    SECTION("insert or assign")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto [itr, inserted] = fm.insert_or_assign(8, 9);
            REQUIRE(inserted);
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto [itr, inserted] = fm.insert_or_assign(3, 4);
            REQUIRE(inserted);
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto [itr, inserted] = fm.insert_or_assign(2, 5);
            REQUIRE_FALSE(inserted);
            REQUIRE(*itr == MAKE_PAIR(2, 5));
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert or assign with helpful hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.insert_or_assign(fm.end(), 8, 9); // 7
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto itr = fm.insert_or_assign(std::next(fm.begin(), 2), 3, 4); // 1
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.insert_or_assign(std::next(fm.begin(), 1), 2, 5); // 4
            REQUIRE(*itr == MAKE_PAIR(2, 5));
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert or assign with annoying hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.insert_or_assign(std::next(fm.begin()), 5, 6); // 5
            REQUIRE(*itr == MAKE_PAIR(5, 6));
            REQUIRE(std::distance(fm.begin(), itr) == 3);
        }

        {
            auto itr = fm.insert_or_assign(fm.end(), 3, 4); // 8
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.insert_or_assign(std::next(fm.begin(), 3), 1, 2); // 2
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(1, 2));
            REQUIRE(std::distance(fm.begin(), itr) == 1);
        }

        {
            auto itr = fm.insert_or_assign(std::next(fm.begin(), 2), 2, 5); // 3
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(2, 5));
        }

        {
            auto itr = fm.insert_or_assign(std::next(fm.begin()), 6, 9); // 6
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(6, 9));
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(5, 6));
        REQUIRE(*itr++ == MAKE_PAIR(6, 9));
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("emplace insertion", "[insertion]")
{
    SECTION("try emplace")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto [itr, inserted] = fm.try_emplace(8, 9);
            REQUIRE(inserted);
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto [itr, inserted] = fm.try_emplace(3, 4);
            REQUIRE(inserted);
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto [itr, inserted] = fm.try_emplace(2, 5);
            REQUIRE_FALSE(inserted);
            REQUIRE(*itr == MAKE_PAIR(2, 3));
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("try emplace with helpful hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.try_emplace(fm.end(), 8, 9); // 7
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto itr = fm.try_emplace(std::next(fm.begin(), 2), 3, 4); // 1
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.try_emplace(std::next(fm.begin(), 1), 2, 5); // 4
            REQUIRE(*itr == MAKE_PAIR(2, 3));
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("try emplace with annoying hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.try_emplace(std::next(fm.begin()), 5, 6); // 5
            REQUIRE(*itr == MAKE_PAIR(5, 6));
            REQUIRE(std::distance(fm.begin(), itr) == 3);
        }

        {
            auto itr = fm.try_emplace(fm.end(), 3, 4); // 8
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.try_emplace(std::next(fm.begin(), 3), 1, 2); // 2
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(1, 2));
            REQUIRE(std::distance(fm.begin(), itr) == 1);
        }

        {
            auto itr = fm.try_emplace(std::next(fm.begin(), 2), 2, 5); // 3
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(2, 3));
        }

        {
            auto itr = fm.try_emplace(std::next(fm.begin()), 6, 9); // 6
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(6, 7));
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(5, 6));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }

    SECTION("piecewise insertion")
    {
        FLAT_CONTAINER<int, Value> fm;

        fm.try_emplace(3, 0xdeadbeef, "deadbeef");
        fm.try_emplace(1, 0xc0ffee, "c0ffee");

        REQUIRE(fm[1].value == 0xc0ffee);
        REQUIRE(!strcmp(fm[1].name, "c0ffee"));

        REQUIRE(fm[3].value == (int)0xdeadbeef);
        REQUIRE(!strcmp(fm[3].name, "deadbeef"));
    }
}
