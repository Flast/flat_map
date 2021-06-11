// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>
#include <functional>
#include <iterator>
#include <vector>

#include "config.hpp"

template <typename T>
struct wrap
{
    T value;
    wrap(T value) : value(value) {}
};

template <typename T> bool operator<(T lhs, wrap<T> rhs) { return lhs < rhs.value; }
template <typename T> bool operator<(wrap<T> lhs, T rhs) { return lhs.value < rhs; }

TEST_CASE("construction", "[construction]")
{
    SECTION("default construction")
    {
        FLAT_CONTAINER<int, int> fm;
    }

    SECTION("iter construction")
    {
        std::vector v =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(2, 5),
            MAKE_PAIR(0, 1),
        };
        FLAT_CONTAINER<int, int> fm{v.begin(), v.end()};

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }

    SECTION("init list construction")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(2, 5),
            MAKE_PAIR(0, 1),
        };

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }

    SECTION("copy construction")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(0, 1),
        };
        auto* ptr = &*fm.begin();

        auto copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        auto itr = copy.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == copy.end());
    }

    SECTION("move construction")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(0, 1),
        };
        auto* ptr = &*fm.begin();

        auto move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        auto itr = move.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("assignment", "[assignment]")
{
    SECTION("copy assignment")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(0, 1),
        };
        auto* ptr = &*fm.begin();

        FLAT_CONTAINER<int, int> copy =
        {
            MAKE_PAIR(1, 1),
        };
        copy = fm;

        REQUIRE(ptr == &*fm.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fm.begin());

        auto itr = copy.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == copy.end());
    }

    SECTION("move assignment")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(6, 7),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(0, 1),
        };
        auto* ptr = &*fm.begin();

        FLAT_CONTAINER<int, int> move =
        {
            MAKE_PAIR(1, 1),
        };
        move = std::move(fm);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fm.begin());
        REQUIRE(fm.begin() == fm.end());

        auto itr = move.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == move.end());
    }
}

TEST_CASE("size", "[size]")
{
    SECTION("empty")
    {
        FLAT_CONTAINER<int, int> fm;

        REQUIRE(fm.empty());
        REQUIRE(fm.size() == 0);
    }

    SECTION("non empty")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(1, 1),
        };

        REQUIRE_FALSE(fm.empty());
        REQUIRE(fm.size() == 1);
    }

    SECTION("clear")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(1, 1),
        };

        fm.clear();
        REQUIRE(fm.empty());
    }
}

TEST_CASE("equal_range", "[equal_range]")
{
    FLAT_CONTAINER<int, int> const fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("not found")
    {
        auto [first, last] = fm.equal_range(3);
        REQUIRE(first != fm.end());
        REQUIRE(first == last);
        REQUIRE(*first == MAKE_PAIR(4, 5));
    }

    SECTION("found")
    {
        auto [first, last] = fm.equal_range(2);
        REQUIRE(first != fm.end());
#if MULTI_CONTAINER
        REQUIRE(std::distance(first, last) == 2);
        REQUIRE(*first == MAKE_PAIR(2, 3));
        REQUIRE(*std::next(first) == MAKE_PAIR(2, 9));
#else
        REQUIRE(std::next(first) == last);
        REQUIRE(*first == MAKE_PAIR(2, 3));
#endif
    }
}

TEST_CASE("equal_range with transparent", "[equal_range]")
{
    FLAT_CONTAINER<int, int, std::less<>> const fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("not found")
    {
        auto [first, last] = fm.equal_range(wrap{3});
        REQUIRE(first != fm.end());
        REQUIRE(first == last);
        REQUIRE(*first == MAKE_PAIR(4, 5));
    }

    SECTION("found")
    {
        auto [first, last] = fm.equal_range(wrap{2});
        REQUIRE(first != fm.end());
#if MULTI_CONTAINER
        REQUIRE(std::distance(first, last) == 2);
        REQUIRE(*first == MAKE_PAIR(2, 3));
        REQUIRE(*std::next(first) == MAKE_PAIR(2, 9));
#else
        REQUIRE(std::next(first) == last);
        REQUIRE(*first == MAKE_PAIR(2, 3));
#endif
    }
}

TEST_CASE("lower_bound", "[lower_bound]")
{
    FLAT_CONTAINER<int, int> const fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("not equal")
    {
        auto itr = fm.lower_bound(3);
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(4, 5));
    }

    SECTION("equal")
    {
        auto itr = fm.lower_bound(2);
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(2, 3));
    }

    SECTION("not found")
    {
        auto itr = fm.lower_bound(9);
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("lower_bound with transparent", "[lower_bound]")
{
    FLAT_CONTAINER<int, int, std::less<>> const fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("not equal")
    {
        auto itr = fm.lower_bound(wrap{3});
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(4, 5));
    }

    SECTION("equal")
    {
        auto itr = fm.lower_bound(wrap{2});
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(2, 3));
    }

    SECTION("not found")
    {
        auto itr = fm.lower_bound(wrap{9});
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("upper_bound", "[upper_bound]")
{
    FLAT_CONTAINER<int, int> const fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("not equal")
    {
        auto itr = fm.upper_bound(3);
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(4, 5));
    }

    SECTION("equal")
    {
        auto itr = fm.upper_bound(2);
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(4, 5));
    }

    SECTION("not found")
    {
        auto itr = fm.upper_bound(9);
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("upper_bound with transparent", "[upper_bound]")
{
    FLAT_CONTAINER<int, int, std::less<>> const fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("not equal")
    {
        auto itr = fm.upper_bound(wrap{3});
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(4, 5));
    }

    SECTION("equal")
    {
        auto itr = fm.upper_bound(wrap{2});
        REQUIRE(itr != fm.end());
        REQUIRE(*itr == MAKE_PAIR(4, 5));
    }

    SECTION("not found")
    {
        auto itr = fm.upper_bound(wrap{9});
        REQUIRE(itr == fm.end());
    }
}

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

    SECTION("found")
    {
        auto itr = fm.find(2);
        REQUIRE(itr != fm.end());
        REQUIRE(std::distance(fm.begin(), itr) == 1);
        REQUIRE(*itr == MAKE_PAIR(2, 3));
    }

    SECTION("not found")
    {
        auto itr = fm.find(3);
        REQUIRE(itr == fm.end());
    }

    SECTION("counting")
    {
#if MULTI_CONTAINER
        REQUIRE(fm.count(2) == 2);
#else
        REQUIRE(fm.count(2) == 1);
#endif
        REQUIRE(fm.count(3) == 0);
    }

    SECTION("contains")
    {
        REQUIRE(fm.contains(4));
        REQUIRE_FALSE(fm.contains(5));
    }
}

TEST_CASE("accessor with transparent", "[accessor]")
{
    FLAT_CONTAINER<int, int, std::less<>> fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(2, 9),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    SECTION("found")
    {
        auto itr = fm.find(wrap{2});
        REQUIRE(itr != fm.end());
        REQUIRE(std::distance(fm.begin(), itr) == 1);
        REQUIRE(*itr == MAKE_PAIR(2, 3));
    }

    SECTION("not found")
    {
        auto itr = fm.find(wrap{3});
        REQUIRE(itr == fm.end());
    }

    SECTION("counting")
    {
#if MULTI_CONTAINER
        REQUIRE(fm.count(wrap{2}) == 2);
#else
        REQUIRE(fm.count(wrap{2}) == 1);
#endif
        REQUIRE(fm.count(wrap{3}) == 0);
    }

    SECTION("contains")
    {
        REQUIRE(fm.contains(wrap{4}));
        REQUIRE_FALSE(fm.contains(wrap{5}));
    }
}

TEST_CASE("insertion", "[insertion]")
{
    SECTION("insert")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto res = fm.insert(MAKE_PAIR(8, 9));
            REQUIRE_INSERTED(res);
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), INSERT_ITR(res)) == 4);
        }

        {
            auto res = fm.insert(MAKE_PAIR(3, 4));
            REQUIRE_INSERTED(res);
            REQUIRE(fm.size() == 6);
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), INSERT_ITR(res)) == 2);
        }

        {
            auto res = fm.insert(MAKE_PAIR(2, 5));
            REQUIRE_INSERTED_FALSE(res);
#if MULTI_CONTAINER
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(2, 5));
#else
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(2, 3));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert with helpful hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.insert(fm.end(), MAKE_PAIR(8, 9)); // 7
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 2), MAKE_PAIR(3, 4)); // 1
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 1), MAKE_PAIR(2, 5)); // 4
#if MULTI_CONTAINER
            REQUIRE(*itr == MAKE_PAIR(2, 5));
#else
            REQUIRE(*itr == MAKE_PAIR(2, 3));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert with annoying hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.insert(std::next(fm.begin()), MAKE_PAIR(5, 6)); // 5
            REQUIRE(*itr == MAKE_PAIR(5, 6));
            REQUIRE(std::distance(fm.begin(), itr) == 3);
        }

        {
            auto itr = fm.insert(fm.end(), MAKE_PAIR(3, 4)); // 8
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 3), MAKE_PAIR(1, 2)); // 2
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(1, 2));
            REQUIRE(std::distance(fm.begin(), itr) == 1);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 2), MAKE_PAIR(2, 5)); // 3
#if MULTI_CONTAINER
            REQUIRE(fm.size() == 8);
            REQUIRE(*itr == MAKE_PAIR(2, 5));
#else
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(2, 3));
#endif
        }

        {
            auto itr = fm.insert(std::next(fm.begin()), MAKE_PAIR(6, 9)); // 6
#if MULTI_CONTAINER
            REQUIRE(fm.size() == 9);
            REQUIRE(*itr == MAKE_PAIR(6, 9));
#else
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(6, 7));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(5, 6));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 9));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert range")
    {
        std::vector v =
        {
            MAKE_PAIR(3, 9),
            MAKE_PAIR(1, 3),
            MAKE_PAIR(6, 4),
            MAKE_PAIR(1, 2),
        };
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        fm.insert(v.begin(), v.end());

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 8);
#else
        REQUIRE(fm.size() == 6);
#endif

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 9));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 4));
#endif
        REQUIRE(itr == fm.end());
    }

    SECTION("insert initializer_list")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        fm.insert({
            MAKE_PAIR(3, 9),
            MAKE_PAIR(1, 3),
            MAKE_PAIR(6, 4),
            MAKE_PAIR(1, 2),
        });

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 8);
#else
        REQUIRE(fm.size() == 6);
#endif

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 9));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 4));
#endif
        REQUIRE(itr == fm.end());
    }

    SECTION("insert sorted range")
    {
        std::vector v =
        {
            MAKE_PAIR(1, 3),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(3, 9),
            MAKE_PAIR(6, 4),
            MAKE_PAIR(7, 8),
            MAKE_PAIR(7, 9),
        };
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        fm.insert_sorted(v.begin(), v.end());

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 10);
#else
        REQUIRE(fm.size() == 7);
#endif

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 9));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 4));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(7, 8));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(7, 9));
#endif
        REQUIRE(itr == fm.end());
    }

    SECTION("insert sorted initializer_list")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
            MAKE_PAIR(7, 8),
            MAKE_PAIR(7, 9),
        };

        fm.insert_sorted({
            MAKE_PAIR(1, 3),
            MAKE_PAIR(1, 2),
            MAKE_PAIR(3, 9),
            MAKE_PAIR(6, 4),
        });

#if MULTI_CONTAINER
        REQUIRE(fm.size() == 10);
#else
        REQUIRE(fm.size() == 7);
#endif

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 9));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 4));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(7, 8));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(7, 9));
#endif
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("erase", "[erase]")
{
    SECTION("erase by key")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        REQUIRE(fm.erase(5) == 0);
        REQUIRE(fm.size() == 4);

        REQUIRE(fm.erase(2) == 1);
        REQUIRE(fm.size() == 3);

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("node", "[insertion][erase]")
{
    SECTION("extract")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(2, 1),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        auto const size = fm.size();
        auto node = fm.extract(5);
        REQUIRE(fm.size() == size);
        REQUIRE_FALSE(node.value.has_value());

        node = fm.extract(2);
        REQUIRE(fm.size() == (size - 1));
        REQUIRE(node.value.has_value());
        REQUIRE(*node.value == MAKE_PAIR(2, 3));

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 1));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }

    using node_type = FLAT_CONTAINER<int, int>::node_type;

    SECTION("insert")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto res = fm.insert(node_type{MAKE_PAIR(8, 9)});
            REQUIRE_NODE_INSERTED(res);
#if !MULTI_CONTAINER
            REQUIRE_FALSE(res.node.value.has_value());
#endif
            REQUIRE(*NODE_INSERT_ITR(res) == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), NODE_INSERT_ITR(res)) == 4);
        }

        {
            auto res = fm.insert(node_type{MAKE_PAIR(3, 4)});
            REQUIRE_NODE_INSERTED(res);
#if !MULTI_CONTAINER
            REQUIRE_FALSE(res.node.value.has_value());
#endif
            REQUIRE(fm.size() == 6);
            REQUIRE(*NODE_INSERT_ITR(res) == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), NODE_INSERT_ITR(res)) == 2);
        }

        {
            auto res = fm.insert(node_type{MAKE_PAIR(2, 5)});
            REQUIRE_NODE_INSERTED_FALSE(res);
#if MULTI_CONTAINER
            REQUIRE(*res == MAKE_PAIR(2, 5));
#else
            REQUIRE(*res.node.value == MAKE_PAIR(2, 5));
            REQUIRE(*res.position == MAKE_PAIR(2, 3));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert with helpful hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.insert(fm.end(), node_type{MAKE_PAIR(8, 9)}); // 7
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 2), node_type{MAKE_PAIR(3, 4)}); // 1
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 1), node_type{MAKE_PAIR(2, 5)}); // 4
#if MULTI_CONTAINER
            REQUIRE(*itr == MAKE_PAIR(2, 5));
#else
            REQUIRE(*itr == MAKE_PAIR(2, 3));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("insert with annoying hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.insert(std::next(fm.begin()), node_type{MAKE_PAIR(5, 6)}); // 5
            REQUIRE(*itr == MAKE_PAIR(5, 6));
            REQUIRE(std::distance(fm.begin(), itr) == 3);
        }

        {
            auto itr = fm.insert(fm.end(), node_type{MAKE_PAIR(3, 4)}); // 8
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 3), node_type{MAKE_PAIR(1, 2)}); // 2
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(1, 2));
            REQUIRE(std::distance(fm.begin(), itr) == 1);
        }

        {
            auto itr = fm.insert(std::next(fm.begin(), 2), node_type{MAKE_PAIR(2, 5)}); // 3
#if MULTI_CONTAINER
            REQUIRE(fm.size() == 8);
            REQUIRE(*itr == MAKE_PAIR(2, 5));
#else
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(2, 3));
#endif
        }

        {
            auto itr = fm.insert(std::next(fm.begin()), node_type{MAKE_PAIR(6, 9)}); // 6
#if MULTI_CONTAINER
            REQUIRE(fm.size() == 9);
            REQUIRE(*itr == MAKE_PAIR(6, 9));
#else
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(6, 7));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(5, 6));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 9));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("emplace insertion", "[insertion]")
{
    SECTION("emplace")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto res = fm.emplace(PAIR_PARAM(8, 9));
            REQUIRE_INSERTED(res);
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), INSERT_ITR(res)) == 4);
        }

        {
            auto res = fm.emplace(PAIR_PARAM(3, 4));
            REQUIRE_INSERTED(res);
            REQUIRE(fm.size() == 6);
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), INSERT_ITR(res)) == 2);
        }

        {
            auto res = fm.emplace(PAIR_PARAM(2, 5));
            REQUIRE_INSERTED_FALSE(res);
#if MULTI_CONTAINER
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(2, 5));
#else
            REQUIRE(*INSERT_ITR(res) == MAKE_PAIR(2, 3));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("emplace with helpful hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.emplace_hint(fm.end(), PAIR_PARAM(8, 9)); // 7
            REQUIRE(*itr == MAKE_PAIR(8, 9));
            REQUIRE(std::distance(fm.begin(), itr) == 4);
        }

        {
            auto itr = fm.emplace_hint(std::next(fm.begin(), 2), PAIR_PARAM(3, 4)); // 1
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.emplace_hint(std::next(fm.begin(), 1), PAIR_PARAM(2, 5)); // 4
#if MULTI_CONTAINER
            REQUIRE(*itr == MAKE_PAIR(2, 5));
#else
            REQUIRE(*itr == MAKE_PAIR(2, 3));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(*itr++ == MAKE_PAIR(8, 9));
        REQUIRE(itr == fm.end());
    }

    SECTION("emplace with annoying hint")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        {
            auto itr = fm.emplace_hint(std::next(fm.begin()), PAIR_PARAM(5, 6)); // 5
            REQUIRE(*itr == MAKE_PAIR(5, 6));
            REQUIRE(std::distance(fm.begin(), itr) == 3);
        }

        {
            auto itr = fm.emplace_hint(fm.end(), PAIR_PARAM(3, 4)); // 8
            REQUIRE(fm.size() == 6);
            REQUIRE(*itr == MAKE_PAIR(3, 4));
            REQUIRE(std::distance(fm.begin(), itr) == 2);
        }

        {
            auto itr = fm.emplace_hint(std::next(fm.begin(), 3), PAIR_PARAM(1, 2)); // 2
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(1, 2));
            REQUIRE(std::distance(fm.begin(), itr) == 1);
        }

        {
            auto itr = fm.emplace_hint(std::next(fm.begin(), 2), PAIR_PARAM(2, 5)); // 3
#if MULTI_CONTAINER
            REQUIRE(fm.size() == 8);
            REQUIRE(*itr == MAKE_PAIR(2, 5));
#else
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(2, 3));
#endif
        }

        {
            auto itr = fm.emplace_hint(std::next(fm.begin()), PAIR_PARAM(6, 9)); // 6
#if MULTI_CONTAINER
            REQUIRE(fm.size() == 9);
            REQUIRE(*itr == MAKE_PAIR(6, 9));
#else
            REQUIRE(fm.size() == 7);
            REQUIRE(*itr == MAKE_PAIR(6, 7));
#endif
        }

        auto itr = fm.begin();
        REQUIRE(*itr++ == MAKE_PAIR(0, 1));
        REQUIRE(*itr++ == MAKE_PAIR(1, 2));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(2, 5));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(2, 3));
        REQUIRE(*itr++ == MAKE_PAIR(3, 4));
        REQUIRE(*itr++ == MAKE_PAIR(4, 5));
        REQUIRE(*itr++ == MAKE_PAIR(5, 6));
#if MULTI_CONTAINER
        REQUIRE(*itr++ == MAKE_PAIR(6, 9));
#endif
        REQUIRE(*itr++ == MAKE_PAIR(6, 7));
        REQUIRE(itr == fm.end());
    }
}

TEST_CASE("swap", "[swap]")
{
    SECTION("member method")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_CONTAINER<int, int> dst;

        fm.swap(dst);

        REQUIRE(fm.empty());
        REQUIRE(dst.size() == 4);
    }

    SECTION("free function")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        FLAT_CONTAINER<int, int> dst;

        swap(fm, dst);

        REQUIRE(fm.empty());
        REQUIRE(dst.size() == 4);
    }
}

TEST_CASE("erase_if", "[erase_if]")
{
    FLAT_CONTAINER<int, int> fm =
    {
        MAKE_PAIR(0, 1),
        MAKE_PAIR(2, 3),
        MAKE_PAIR(4, 5),
        MAKE_PAIR(6, 7),
    };

    auto n = erase_if(fm, [](auto& kvp) { return FIRST(kvp) < 4; });
    REQUIRE(n == 2);
    REQUIRE(fm.size() == 2);

    auto itr = fm.begin();
    REQUIRE(*itr++ == MAKE_PAIR(4, 5));
    REQUIRE(*itr++ == MAKE_PAIR(6, 7));
    REQUIRE(itr == fm.end());
}

TEST_CASE("comparison", "[comparison]")
{
    SECTION("traditional comparator")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        REQUIRE(fm == FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm != FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
#if FLAT_MAP
        REQUIRE_FALSE(fm == FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm != FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
#endif
        REQUIRE_FALSE(fm == FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm != FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});

        REQUIRE_FALSE(fm < FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm > FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm <= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm >= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
#if FLAT_MAP
        REQUIRE(fm < FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm > FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm <= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm >= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});

        REQUIRE_FALSE(fm < FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 2), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm > FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 2), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm <= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 2), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm >= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 2), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
#endif
        REQUIRE_FALSE(fm < FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm > FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm <= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm >= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});

        REQUIRE(fm < FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(3, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm > FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(3, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE(fm <= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(3, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
        REQUIRE_FALSE(fm >= FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(3, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)});
    }

#ifdef FLAT_MAP_HAS_THREE_WAY_COMPARISON
    SECTION("three way comparison")
    {
        FLAT_CONTAINER<int, int> fm =
        {
            MAKE_PAIR(0, 1),
            MAKE_PAIR(2, 3),
            MAKE_PAIR(4, 5),
            MAKE_PAIR(6, 7),
        };

        REQUIRE((fm <=> FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}) == std::weak_ordering::equivalent);
#if FLAT_MAP
        REQUIRE((fm <=> FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 4), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}) == std::weak_ordering::less);
        REQUIRE((fm <=> FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(2, 2), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}) == std::weak_ordering::greater);
#endif
        REQUIRE((fm <=> FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(3, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}) == std::weak_ordering::less);
        REQUIRE((fm <=> FLAT_CONTAINER<int, int>{MAKE_PAIR(0, 1), MAKE_PAIR(1, 3), MAKE_PAIR(4, 5), MAKE_PAIR(6, 7)}) == std::weak_ordering::greater);
    }
#endif
}
