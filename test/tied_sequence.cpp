// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>
#include <stdexcept>
#include <vector>

#include "flat_map/tied_sequence.hpp"

TEST_CASE("construction", "[construction]")
{
    SECTION("default construction")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
    }

    SECTION("count copies")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts(4, {1, 2});
        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{1, 2});
        REQUIRE(ts[1] == std::tuple{1, 2});
        REQUIRE(ts[2] == std::tuple{1, 2});
        REQUIRE(ts[3] == std::tuple{1, 2});
    }

    SECTION("count default-initialized")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts(4);
        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple<int, int>{});
        REQUIRE(ts[1] == std::tuple<int, int>{});
        REQUIRE(ts[2] == std::tuple<int, int>{});
        REQUIRE(ts[3] == std::tuple<int, int>{});
    }

    SECTION("from sequence")
    {
        std::vector v =
        {
            std::tuple{0, 1},
            std::tuple{2, 3},
            std::tuple{4, 5},
            std::tuple{6, 7},
        };

        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts(v.begin(), v.end());
        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }

    SECTION("copy ctor")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> src(4, {1, 2});
        auto dst = src;

        REQUIRE(src.begin() != dst.begin());
        REQUIRE(src.data() != dst.data());
        REQUIRE(src.size() == 4);
        REQUIRE(src[0] == std::tuple{1, 2});
        REQUIRE(src[1] == std::tuple{1, 2});
        REQUIRE(src[2] == std::tuple{1, 2});
        REQUIRE(src[3] == std::tuple{1, 2});
        REQUIRE(dst.size() == 4);
        REQUIRE(dst[0] == std::tuple{1, 2});
        REQUIRE(dst[1] == std::tuple{1, 2});
        REQUIRE(dst[2] == std::tuple{1, 2});
        REQUIRE(dst[3] == std::tuple{1, 2});
    }

    SECTION("move ctor")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> src(4, {1, 2});
        auto dst = std::move(src);

        REQUIRE(src.begin() != dst.begin());
        REQUIRE(src.empty());
        REQUIRE(dst.size() == 4);
        REQUIRE(dst[0] == std::tuple{1, 2});
        REQUIRE(dst[1] == std::tuple{1, 2});
        REQUIRE(dst[2] == std::tuple{1, 2});
        REQUIRE(dst[3] == std::tuple{1, 2});
    }

    SECTION("initializer_list")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }
}

TEST_CASE("assignment", "[assignment]")
{
    SECTION("copy assignment")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> src(4, {1, 2}), dst;
        dst = src;

        REQUIRE(src.begin() != dst.begin());
        REQUIRE(src.size() == 4);
        REQUIRE(src[0] == std::tuple{1, 2});
        REQUIRE(src[1] == std::tuple{1, 2});
        REQUIRE(src[2] == std::tuple{1, 2});
        REQUIRE(src[3] == std::tuple{1, 2});
        REQUIRE(dst.size() == 4);
        REQUIRE(dst[0] == std::tuple{1, 2});
        REQUIRE(dst[1] == std::tuple{1, 2});
        REQUIRE(dst[2] == std::tuple{1, 2});
        REQUIRE(dst[3] == std::tuple{1, 2});
    }

    SECTION("move assignment")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> src(4, {1, 2}), dst;
        dst = std::move(src);

        REQUIRE(src.begin() != dst.begin());
        REQUIRE(src.empty());
        REQUIRE(dst.size() == 4);
        REQUIRE(dst[0] == std::tuple{1, 2});
        REQUIRE(dst[1] == std::tuple{1, 2});
        REQUIRE(dst[2] == std::tuple{1, 2});
        REQUIRE(dst[3] == std::tuple{1, 2});
    }

    SECTION("initializer_list")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }

    SECTION("count copies")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        ts.assign(4, {1, 2});

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{1, 2});
        REQUIRE(ts[1] == std::tuple{1, 2});
        REQUIRE(ts[2] == std::tuple{1, 2});
        REQUIRE(ts[3] == std::tuple{1, 2});
    }

    SECTION("from sequence")
    {
        std::vector v =
        {
            std::tuple{0, 1},
            std::tuple{2, 3},
            std::tuple{4, 5},
            std::tuple{6, 7},
        };

        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        ts.assign(v.begin(), v.end());

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }

    SECTION("assign initializer_list")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        ts.assign(
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        });

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }
}

TEST_CASE("accessor", "[accessor]")
{
    SECTION("at")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts.at(0) == std::tuple{0, 1});
        REQUIRE(ts.at(1) == std::tuple{2, 3});
        REQUIRE(ts.at(2) == std::tuple{4, 5});
        REQUIRE(ts.at(3) == std::tuple{6, 7});
        REQUIRE_THROWS_AS(ts.at(4), std::out_of_range);
    }

    SECTION("operator[]")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }

    SECTION("front back")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts.front() == std::tuple{0, 1});
        REQUIRE(ts.back() == std::tuple{6, 7});
    }

    SECTION("data")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(std::get<0>(ts.data()) != nullptr);
        REQUIRE(std::get<1>(ts.data()) != nullptr);
    }
}

TEST_CASE("iterator", "[iterator]")
{
    SECTION("empty")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        REQUIRE(ts.begin() == ts.end());
        REQUIRE(ts.rbegin() == ts.rend());
    }

    SECTION("non empty")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts.begin() != ts.end());
        REQUIRE(ts.rbegin() != ts.rend());
        REQUIRE(ts.rbegin().base() == ts.end());
        REQUIRE(ts.rend().base() == ts.begin());

        REQUIRE(std::next(ts.begin(), 4) == ts.end());
        REQUIRE(std::next(ts.rbegin(), 4) == ts.rend());
        REQUIRE(std::next(ts.rbegin(), 4).base() == ts.begin());
    }
}

TEST_CASE("capacity", "[capacity]")
{
    SECTION("empty")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        REQUIRE(ts.empty());
        REQUIRE(ts.size() == 0);
        REQUIRE(ts.max_size() > 0);
    }

    SECTION("non empty")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };
        REQUIRE_FALSE(ts.empty());
        REQUIRE(ts.size() == 4);
        REQUIRE(ts.max_size() > 0);
    }

#if 0 // TODO
    SECTION("reserve/capacity")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;
        REQUIRE(ts.capacity() == 0);
        REQUIRE(ts.size() == 0);
        ts.reserve(10);
        REQUIRE(ts.capacity() == 10);
        REQUIRE(ts.size() == 0);
    }

    SECTION("shrink_to_fit")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;

        ts.reserve(10);
        ts.push_back({0, 1});
        REQUIRE(ts.capacity() == 10);
        REQUIRE(ts.size() == 1);

        ts.shrink_to_fit();
        REQUIRE(ts.capacity() == 1);
        REQUIRE(ts.size() == 1);
    }
#endif

    SECTION("clear")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };
        ts.clear();
        REQUIRE(ts.size() == 0);
    }
}

TEST_CASE("insertion", "[insertion]")
{
    SECTION("single")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto v = decltype(ts)::value_type{3, 4};
        auto itr = ts.insert(std::next(ts.begin(), 2), v);
        REQUIRE(itr == std::next(ts.begin(), 2));

        itr = ts.insert(std::next(ts.begin(), 4), {5, 6});
        REQUIRE(itr == std::next(ts.begin(), 4));

        REQUIRE(ts.size() == 6);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{3, 4});
        REQUIRE(ts[3] == std::tuple{4, 5});
        REQUIRE(ts[4] == std::tuple{5, 6});
        REQUIRE(ts[5] == std::tuple{6, 7});
    }

    SECTION("count copies")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto itr = ts.insert(std::next(ts.begin(), 2), 3, {3, 4});
        REQUIRE(itr == std::next(ts.begin(), 2));

        REQUIRE(ts.size() == 7);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{3, 4});
        REQUIRE(ts[3] == std::tuple{3, 4});
        REQUIRE(ts[4] == std::tuple{3, 4});
        REQUIRE(ts[5] == std::tuple{4, 5});
        REQUIRE(ts[6] == std::tuple{6, 7});
    }

    SECTION("from sequence")
    {
        std::vector v =
        {
            std::tuple{2, 3},
            std::tuple{4, 5},
        };

        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {6, 7},
        };
        auto itr = ts.insert(std::next(ts.begin()), v.begin(), v.end());
        REQUIRE(itr == std::next(ts.begin()));

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }

    SECTION("initializer_list")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {6, 7},
        };
        auto itr = ts.insert(std::next(ts.begin()),
        {
            std::tuple{2, 3},
            std::tuple{4, 5},
        });
        REQUIRE(itr == std::next(ts.begin()));

        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
        REQUIRE(ts[3] == std::tuple{6, 7});
    }

    SECTION("emplace")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto itr = ts.emplace(std::next(ts.begin(), 2), 3, 4);
        REQUIRE(itr == std::next(ts.begin(), 2));

        REQUIRE(ts.size() == 5);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{3, 4});
        REQUIRE(ts[3] == std::tuple{4, 5});
        REQUIRE(ts[4] == std::tuple{6, 7});
    }
}

TEST_CASE("erase", "[erase]")
{
    SECTION("single")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto itr = ts.erase(std::next(ts.begin()));
        REQUIRE(itr == std::next(ts.begin()));

        REQUIRE(ts.size() == 3);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{4, 5});
        REQUIRE(ts[2] == std::tuple{6, 7});
    }

    SECTION("range")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto itr = ts.erase(std::next(ts.begin(), 2), ts.end());
        REQUIRE(itr == ts.end());

        REQUIRE(ts.size() == 2);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
    }
}

TEST_CASE("tail insertion", "[insertion]")
{
    SECTION("push_back")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;

        auto v = decltype(ts)::value_type{1, 2};
        ts.push_back(v);
        ts.push_back(decltype(ts)::value_type{3, 4});

        REQUIRE(ts.size() == 2);
        REQUIRE(ts[0] == std::tuple{1, 2});
        REQUIRE(ts[1] == std::tuple{3, 4});
    }

    SECTION("emplace_back")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;

        auto ref = ts.emplace_back(1, 2);
        REQUIRE(ref == std::tuple{1, 2});

        REQUIRE(ts.size() == 1);
        REQUIRE(ts[0] == std::tuple{1, 2});
    }
}

TEST_CASE("tail erasure", "[erase]")
{
    SECTION("pop_back")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        ts.pop_back();
        REQUIRE(ts.size() == 3);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{2, 3});
        REQUIRE(ts[2] == std::tuple{4, 5});
    }
}

TEST_CASE("resize", "[resize]")
{
    SECTION("default")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;

        ts.resize(4);
        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{0, 0});
        REQUIRE(ts[1] == std::tuple{0, 0});
        REQUIRE(ts[2] == std::tuple{0, 0});
        REQUIRE(ts[3] == std::tuple{0, 0});
    }

    SECTION("value")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts;

        ts.resize(4, {1, 2});
        REQUIRE(ts.size() == 4);
        REQUIRE(ts[0] == std::tuple{1, 2});
        REQUIRE(ts[1] == std::tuple{1, 2});
        REQUIRE(ts[2] == std::tuple{1, 2});
        REQUIRE(ts[3] == std::tuple{1, 2});
    }
}

TEST_CASE("swap", "[swap]")
{
    SECTION("member")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> src =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        }, dst;

        src.swap(dst);

        REQUIRE(src.empty());
        REQUIRE(dst.size() == 4);
        REQUIRE(dst[0] == std::tuple{0, 1});
        REQUIRE(dst[1] == std::tuple{2, 3});
        REQUIRE(dst[2] == std::tuple{4, 5});
        REQUIRE(dst[3] == std::tuple{6, 7});
    }

    SECTION("free function")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> src =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        }, dst;

        swap(src, dst);

        REQUIRE(src.empty());
        REQUIRE(dst.size() == 4);
        REQUIRE(dst[0] == std::tuple{0, 1});
        REQUIRE(dst[1] == std::tuple{2, 3});
        REQUIRE(dst[2] == std::tuple{4, 5});
        REQUIRE(dst[3] == std::tuple{6, 7});
    }
}

TEST_CASE("comparator", "[comparator]")
{
    SECTION("traditional comparator")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE(ts == decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts != decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}});

        REQUIRE_FALSE(ts == decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}});
        REQUIRE(ts != decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}});

        REQUIRE_FALSE(ts == decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}});
        REQUIRE(ts != decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}});

        REQUIRE_FALSE(ts < decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts > decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}});
        REQUIRE(ts <= decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}});
        REQUIRE(ts >= decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}});

        REQUIRE(ts < decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts > decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}});
        REQUIRE(ts <= decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts >= decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}});

        REQUIRE_FALSE(ts < decltype(ts){{0, 1}, {2, 2}, {4, 5}, {6, 7}});
        REQUIRE(ts > decltype(ts){{0, 1}, {2, 2}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts <= decltype(ts){{0, 1}, {2, 2}, {4, 5}, {6, 7}});
        REQUIRE(ts >= decltype(ts){{0, 1}, {2, 2}, {4, 5}, {6, 7}});

        REQUIRE_FALSE(ts < decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}});
        REQUIRE(ts > decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts <= decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}});
        REQUIRE(ts >= decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}});

        REQUIRE(ts < decltype(ts){{0, 1}, {3, 3}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts > decltype(ts){{0, 1}, {3, 3}, {4, 5}, {6, 7}});
        REQUIRE(ts <= decltype(ts){{0, 1}, {3, 3}, {4, 5}, {6, 7}});
        REQUIRE_FALSE(ts >= decltype(ts){{0, 1}, {3, 3}, {4, 5}, {6, 7}});
    }

#ifdef FLAT_MAP_HAS_THREE_WAY_COMPARISON
    SECTION("three way comparison")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        REQUIRE((ts <=> decltype(ts){{0, 1}, {2, 3}, {4, 5}, {6, 7}}) == std::weak_ordering::equivalent);

        REQUIRE((ts <=> decltype(ts){{0, 1}, {2, 4}, {4, 5}, {6, 7}}) == std::weak_ordering::less);
        REQUIRE((ts <=> decltype(ts){{0, 1}, {2, 2}, {4, 5}, {6, 7}}) == std::weak_ordering::greater);

        REQUIRE((ts <=> decltype(ts){{0, 1}, {3, 3}, {4, 5}, {6, 7}}) == std::weak_ordering::less);
        REQUIRE((ts <=> decltype(ts){{0, 1}, {1, 3}, {4, 5}, {6, 7}}) == std::weak_ordering::greater);
    }
#endif
}

TEST_CASE("remove erase", "[erase]")
{
    SECTION("erase")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto r = erase(ts, std::tuple{1, 2});
        REQUIRE(r == 0);
        REQUIRE(ts.size() == 4);

        r = erase(ts, std::tuple{2, 3});
        REQUIRE(r == 1);
        REQUIRE(ts.size() == 3);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{4, 5});
        REQUIRE(ts[2] == std::tuple{6, 7});
    }

    SECTION("erase")
    {
        flat_map::tied_sequence<std::vector<int>, std::vector<int>> ts =
        {
            {0, 1},
            {2, 3},
            {4, 5},
            {6, 7},
        };

        auto r = erase_if(ts, [](auto vt) { return std::get<0>(vt) == 1; });
        REQUIRE(r == 0);
        REQUIRE(ts.size() == 4);

        r = erase_if(ts, [](auto vt) { return std::get<0>(vt) == 2; });
        REQUIRE(r == 1);
        REQUIRE(ts.size() == 3);
        REQUIRE(ts[0] == std::tuple{0, 1});
        REQUIRE(ts[1] == std::tuple{4, 5});
        REQUIRE(ts[2] == std::tuple{6, 7});
    }
}
