// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include "flat_map/flat_set.hpp"

#include <catch2/catch.hpp>
#include <iterator>

template <typename Key, typename Compare = std::less<Key>>
using FLAT_SET = flat_map::flat_set<Key, Compare, CONTAINER<Key>>;

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
        FLAT_SET<int> fs;
    }

    SECTION("iter construction")
    {
        std::vector v =
        {
            6,
            4,
            2,
            0,
        };
        FLAT_SET<int> fs{v.begin(), v.end()};

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("init list construction")
    {
        FLAT_SET<int> fs =
        {
            6,
            4,
            2,
            0,
        };

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("copy construction")
    {
        FLAT_SET<int> fs =
        {
            6,
            4,
            2,
            0,
        };
        auto* ptr = &*fs.begin();

        auto copy = fs;

        REQUIRE(ptr == &*fs.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fs.begin());

        auto itr = copy.begin();
        REQUIRE(*itr == 0);
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
        FLAT_SET<int> fs =
        {
            6,
            4,
            2,
            0,
        };
        auto* ptr = &*fs.begin();

        auto move = std::move(fs);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fs.begin());
        REQUIRE(fs.begin() == fs.end());

        auto itr = move.begin();
        REQUIRE(*itr == 0);
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

TEST_CASE("assignment", "[assignment]")
{
    SECTION("copy assignment")
    {
        FLAT_SET<int> fs =
        {
            6,
            4,
            2,
            0,
        };
        auto* ptr = &*fs.begin();

        FLAT_SET<int> copy =
        {
            1,
        };
        copy = fs;

        REQUIRE(ptr == &*fs.begin());
        REQUIRE(ptr != &*copy.begin());
        REQUIRE(copy.begin() != fs.begin());

        auto itr = copy.begin();
        REQUIRE(*itr == 0);
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
        FLAT_SET<int> fs =
        {
            6,
            4,
            2,
            0,
        };
        auto* ptr = &*fs.begin();

        FLAT_SET<int> move =
        {
            1,
        };
        move = std::move(fs);

        REQUIRE(ptr == &*move.begin());
        REQUIRE(move.begin() != fs.begin());
        REQUIRE(fs.begin() == fs.end());

        auto itr = move.begin();
        REQUIRE(*itr == 0);
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

TEST_CASE("size", "[size]")
{
    SECTION("empty")
    {
        FLAT_SET<int> fs;

        REQUIRE(fs.empty());
        REQUIRE(fs.size() == 0);
    }

    SECTION("non empty")
    {
        FLAT_SET<int> fs =
        {
            1,
        };

        REQUIRE_FALSE(fs.empty());
        REQUIRE(fs.size() == 1);
    }

    SECTION("clear")
    {
        FLAT_SET<int> fs =
        {
            1,
        };

        fs.clear();
        REQUIRE(fs.empty());
    }
}

TEST_CASE("equal_range", "[equal_range]")
{
    FLAT_SET<int> const fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("not found")
    {
        auto [first, last] = fs.equal_range(3);
        REQUIRE(first != fs.end());
        REQUIRE(first == last);
        REQUIRE(*first == 4);
    }

    SECTION("found")
    {
        auto [first, last] = fs.equal_range(2);
        REQUIRE(first != fs.end());
        REQUIRE(std::next(first) == last);
        REQUIRE(*first == 2);
    }
}

TEST_CASE("equal_range with transparent", "[equal_range]")
{
    FLAT_SET<int, std::less<>> const fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("not found")
    {
        auto [first, last] = fs.equal_range(wrap{3});
        REQUIRE(first != fs.end());
        REQUIRE(first == last);
        REQUIRE(*first == 4);
    }

    SECTION("found")
    {
        auto [first, last] = fs.equal_range(wrap{2});
        REQUIRE(first != fs.end());
        REQUIRE(std::next(first) == last);
        REQUIRE(*first == 2);
    }
}

TEST_CASE("lower_bound", "[lower_bound]")
{
    FLAT_SET<int> const fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("not equal")
    {
        auto itr = fs.lower_bound(3);
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 4);
    }

    SECTION("equal")
    {
        auto itr = fs.lower_bound(2);
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 2);
    }

    SECTION("not found")
    {
        auto itr = fs.lower_bound(9);
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("lower_bound with transparent", "[lower_bound]")
{
    FLAT_SET<int, std::less<>> const fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("not equal")
    {
        auto itr = fs.lower_bound(wrap{3});
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 4);
    }

    SECTION("equal")
    {
        auto itr = fs.lower_bound(wrap{2});
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 2);
    }

    SECTION("not found")
    {
        auto itr = fs.lower_bound(wrap{9});
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("upper_bound", "[upper_bound]")
{
    FLAT_SET<int> const fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("not equal")
    {
        auto itr = fs.upper_bound(3);
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 4);
    }

    SECTION("equal")
    {
        auto itr = fs.upper_bound(2);
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 4);
    }

    SECTION("not found")
    {
        auto itr = fs.upper_bound(9);
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("upper_bound with transparent", "[upper_bound]")
{
    FLAT_SET<int, std::less<>> const fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("not equal")
    {
        auto itr = fs.upper_bound(wrap{3});
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 4);
    }

    SECTION("equal")
    {
        auto itr = fs.upper_bound(wrap{2});
        REQUIRE(itr != fs.end());
        REQUIRE(*itr == 4);
    }

    SECTION("not found")
    {
        auto itr = fs.upper_bound(wrap{9});
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("accessor", "[accessor]")
{
    FLAT_SET<int> fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("found")
    {
        auto itr = fs.find(2);
        REQUIRE(itr != fs.end());
        REQUIRE(std::distance(fs.begin(), itr) == 1);
        REQUIRE(*itr == 2);
    }

    SECTION("not found")
    {
        auto itr = fs.find(3);
        REQUIRE(itr == fs.end());
    }

    SECTION("counting")
    {
        REQUIRE(fs.count(2) == 1);
        REQUIRE(fs.count(3) == 0);
    }

    SECTION("contains")
    {
        REQUIRE(fs.contains(4));
        REQUIRE_FALSE(fs.contains(5));
    }
}

TEST_CASE("accessor with transparent", "[accessor]")
{
    FLAT_SET<int, std::less<>> fs =
    {
        0,
        2,
        4,
        6,
    };

    SECTION("found")
    {
        auto itr = fs.find(wrap{2});
        REQUIRE(itr != fs.end());
        REQUIRE(std::distance(fs.begin(), itr) == 1);
        REQUIRE(*itr == 2);
    }

    SECTION("not found")
    {
        auto itr = fs.find(wrap{3});
        REQUIRE(itr == fs.end());
    }

    SECTION("counting")
    {
        REQUIRE(fs.count(2) == 1);
        REQUIRE(fs.count(3) == 0);
    }

    SECTION("contains")
    {
        REQUIRE(fs.contains(wrap{4}));
        REQUIRE_FALSE(fs.contains(wrap{5}));
    }
}

TEST_CASE("insertion", "[insertion]")
{
    SECTION("insert")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto [itr, inserted] = fs.insert(8);
            REQUIRE(inserted);
            REQUIRE(*itr == 8);
            REQUIRE(std::distance(fs.begin(), itr) == 4);
        }

        {
            auto [itr, inserted] = fs.insert(3);
            REQUIRE(inserted);
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto [itr, inserted] = fs.insert(2);
            REQUIRE_FALSE(inserted);
            REQUIRE(*itr == 2);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(*itr == 8);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert with helpful hint")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto itr = fs.insert(fs.end(), 8); // 7
            REQUIRE(*itr == 8);
            REQUIRE(std::distance(fs.begin(), itr) == 4);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 2), 3); // 1
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 1), 2); // 4
            REQUIRE(*itr == 2);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(*itr == 8);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert with annoying hint")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto itr = fs.insert(std::next(fs.begin()), 5); // 5
            REQUIRE(*itr == 5);
            REQUIRE(std::distance(fs.begin(), itr) == 3);
        }

        {
            auto itr = fs.insert(fs.end(), 3); // 8
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 3), 1); // 2
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 1);
            REQUIRE(std::distance(fs.begin(), itr) == 1);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 2), 2); // 3
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 2);
        }

        {
            auto itr = fs.insert(std::next(fs.begin()), 6); // 6
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 6);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 5);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert range")
    {
        std::vector v =
        {
            3,
            1,
            6,
            1,
        };
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        fs.insert(v.begin(), v.end());

        REQUIRE(fs.size() == 6);

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert initializer_list")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        fs.insert({
            3,
            1,
            6,
            1,
        });

        REQUIRE(fs.size() == 6);

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert sorted range")
    {
        std::vector v =
        {
            1,
            1,
            3,
            6,
        };
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        fs.insert_sorted(v.begin(), v.end());

        REQUIRE(fs.size() == 6);

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert sorted initializer_list")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        fs.insert_sorted({
            1,
            1,
            3,
            6,
        });

        REQUIRE(fs.size() == 6);

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("erase", "[erase]")
{
    SECTION("erase by key")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        REQUIRE(fs.erase(5) == 0);
        REQUIRE(fs.size() == 4);

        REQUIRE(fs.erase(2) == 1);
        REQUIRE(fs.size() == 3);

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("node", "[insertion][erase]")
{
    SECTION("extract")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        auto node = fs.extract(5);
        REQUIRE(fs.size() == 4);
        REQUIRE_FALSE(node.value.has_value());

        node = fs.extract(2);
        REQUIRE(fs.size() == 3);
        REQUIRE(node.value.has_value());
        REQUIRE(*node.value == 2);

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    using node_type = FLAT_SET<int>::node_type;

    SECTION("insert")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto [itr, inserted, node] = fs.insert(node_type{8});
            REQUIRE(inserted);
            REQUIRE_FALSE(node.value.has_value());
            REQUIRE(*itr == 8);
            REQUIRE(std::distance(fs.begin(), itr) == 4);
        }

        {
            auto [itr, inserted, node] = fs.insert(node_type{3});
            REQUIRE(inserted);
            REQUIRE_FALSE(node.value.has_value());
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto [itr, inserted, node] = fs.insert(node_type{2});
            REQUIRE_FALSE(inserted);
            REQUIRE(*node.value == 2);
            REQUIRE(*itr == 2);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(*itr == 8);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert with helpful hint")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto itr = fs.insert(fs.end(), node_type{8}); // 7
            REQUIRE(*itr == 8);
            REQUIRE(std::distance(fs.begin(), itr) == 4);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 2), node_type{3}); // 1
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 1), node_type{2}); // 4
            REQUIRE(*itr == 2);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(*itr == 8);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("insert with annoying hint")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto itr = fs.insert(std::next(fs.begin()), node_type{5}); // 5
            REQUIRE(*itr == 5);
            REQUIRE(std::distance(fs.begin(), itr) == 3);
        }

        {
            auto itr = fs.insert(fs.end(), node_type{3}); // 8
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 3), node_type{1}); // 2
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 1);
            REQUIRE(std::distance(fs.begin(), itr) == 1);
        }

        {
            auto itr = fs.insert(std::next(fs.begin(), 2), node_type{2}); // 3
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 2);
        }

        {
            auto itr = fs.insert(std::next(fs.begin()), node_type{6}); // 6
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 6);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 5);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("emplace insertion", "[insertion]")
{
    SECTION("emplace")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto [itr, inserted] = fs.emplace(8);
            REQUIRE(inserted);
            REQUIRE(*itr == 8);
            REQUIRE(std::distance(fs.begin(), itr) == 4);
        }

        {
            auto [itr, inserted] = fs.emplace(3);
            REQUIRE(inserted);
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto [itr, inserted] = fs.emplace(2);
            REQUIRE_FALSE(inserted);
            REQUIRE(*itr == 2);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(*itr == 8);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("emplace with helpful hint")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto itr = fs.emplace_hint(fs.end(), 8); // 7
            REQUIRE(*itr == 8);
            REQUIRE(std::distance(fs.begin(), itr) == 4);
        }

        {
            auto itr = fs.emplace_hint(std::next(fs.begin(), 2), 3); // 1
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto itr = fs.emplace_hint(std::next(fs.begin(), 1), 2); // 4
            REQUIRE(*itr == 2);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(*itr == 8);
        ++itr;
        REQUIRE(itr == fs.end());
    }

    SECTION("emplace with annoying hint")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        {
            auto itr = fs.emplace_hint(std::next(fs.begin()), 5); // 5
            REQUIRE(*itr == 5);
            REQUIRE(std::distance(fs.begin(), itr) == 3);
        }

        {
            auto itr = fs.emplace_hint(fs.end(), 3); // 8
            REQUIRE(fs.size() == 6);
            REQUIRE(*itr == 3);
            REQUIRE(std::distance(fs.begin(), itr) == 2);
        }

        {
            auto itr = fs.emplace_hint(std::next(fs.begin(), 3), 1); // 2
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 1);
            REQUIRE(std::distance(fs.begin(), itr) == 1);
        }

        {
            auto itr = fs.emplace_hint(std::next(fs.begin(), 2), 2); // 3
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 2);
        }

        {
            auto itr = fs.emplace_hint(std::next(fs.begin()), 6); // 6
            REQUIRE(fs.size() == 7);
            REQUIRE(*itr == 6);
        }

        auto itr = fs.begin();
        REQUIRE(*itr == 0);
        ++itr;
        REQUIRE(*itr == 1);
        ++itr;
        REQUIRE(*itr == 2);
        ++itr;
        REQUIRE(*itr == 3);
        ++itr;
        REQUIRE(*itr == 4);
        ++itr;
        REQUIRE(*itr == 5);
        ++itr;
        REQUIRE(*itr == 6);
        ++itr;
        REQUIRE(itr == fs.end());
    }
}

TEST_CASE("swap", "[swap]")
{
    SECTION("member method")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        FLAT_SET<int> dst;

        fs.swap(dst);

        REQUIRE(fs.empty());
        REQUIRE(dst.size() == 4);
    }

    SECTION("free function")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        FLAT_SET<int> dst;

        swap(fs, dst);

        REQUIRE(fs.empty());
        REQUIRE(dst.size() == 4);
    }
}

TEST_CASE("erase_if", "[erase_if]")
{
    FLAT_SET<int> fs =
    {
        0,
        2,
        4,
        6,
    };

    auto n = erase_if(fs, [](int k) { return k < 4; });
    REQUIRE(n == 2);
    REQUIRE(fs.size() == 2);

    auto itr = fs.begin();
    REQUIRE(*itr == 4);
    ++itr;
    REQUIRE(*itr == 6);
    ++itr;
    REQUIRE(itr == fs.end());
}

TEST_CASE("comparison", "[comparison]")
{
    SECTION("traditional comparator")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        REQUIRE(fs == FLAT_SET<int>{0, 2, 4, 6});
        REQUIRE_FALSE(fs != FLAT_SET<int>{0, 2, 4, 6});

        REQUIRE_FALSE(fs == FLAT_SET<int>{0, 1, 4, 6});
        REQUIRE(fs != FLAT_SET<int>{0, 1, 4, 6});

        REQUIRE_FALSE(fs < FLAT_SET<int>{0, 2, 4, 6});
        REQUIRE_FALSE(fs > FLAT_SET<int>{0, 2, 4, 6});
        REQUIRE(fs <= FLAT_SET<int>{0, 2, 4, 6});
        REQUIRE(fs >= FLAT_SET<int>{0, 2, 4, 6});

        REQUIRE_FALSE(fs < FLAT_SET<int>{0, 1, 4, 6});
        REQUIRE(fs > FLAT_SET<int>{0, 1, 4, 6});
        REQUIRE_FALSE(fs <= FLAT_SET<int>{0, 1, 4, 6});
        REQUIRE(fs >= FLAT_SET<int>{0, 1, 4, 6});

        REQUIRE(fs < FLAT_SET<int>{0, 3, 4, 6});
        REQUIRE_FALSE(fs > FLAT_SET<int>{0, 3, 4, 6});
        REQUIRE(fs <= FLAT_SET<int>{0, 3, 4, 6});
        REQUIRE_FALSE(fs >= FLAT_SET<int>{0, 3, 4, 6});
    }

#ifdef FLAT_MAP_HAS_THREE_WAY_COMPARISON
    SECTION("three way comparison")
    {
        FLAT_SET<int> fs =
        {
            0,
            2,
            4,
            6,
        };

        REQUIRE((fs <=> FLAT_SET<int>{0, 2, 4, 6}) == std::weak_ordering::equivalent);
        REQUIRE((fs <=> FLAT_SET<int>{0, 3, 4, 6}) == std::weak_ordering::less);
        REQUIRE((fs <=> FLAT_SET<int>{0, 1, 4, 6}) == std::weak_ordering::greater);
    }
#endif
}
