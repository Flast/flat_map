// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>
#include <functional>
#include <vector>

#include "flat_map/__inplace_unique_sort_merge.hpp"

using flat_map::range_order;

template <typename Container>
auto do_algorithm(Container& cont, typename Container::iterator mid, range_order desire, range_order order)
{
    auto comp = std::less<typename Container::value_type>{};
    return flat_map::detail::inplace_unique_sort_merge(cont.begin(), mid, cont.end(), desire, order, comp, cont.get_allocator());
};

struct key_value_pair
{
    int key;
    int value;
    key_value_pair() : key_value_pair(0) {}
    key_value_pair(int v) : key_value_pair(v, v) {}
    key_value_pair(int key, int value) : key(key), value(value) {}
    bool operator==(key_value_pair const& other) const { return key == other.key && value == other.value; }
    bool operator<(key_value_pair const& other) const { return key < other.key; }
};

std::ostream& operator<<(std::ostream& ostr, key_value_pair const& kvp)
{
    ostr << '{' << kvp.key << ',' << kvp.value << '}';
    return ostr;
}

TEST_CASE("inplace_merge", "[inplace_merge]")
{
    SECTION("unique_sorted -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, 2, 4, 6};
        auto mid = v.insert(v.end(), {1, 3, 5});
        auto itr = do_algorithm(v, mid, range_order::unique_sorted, range_order::unique_sorted);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, 2, 3, 4, 5, 6});
    }

    SECTION("unique_sorted -> sorted")
    {
        std::vector<key_value_pair> v{0, 2, {2,3}, {4,2}, 4, 4, 6, 6};
        auto mid = v.insert(v.end(), {{0,4}, {2,1}, {4,6}, {6,3}});
        auto itr = do_algorithm(v, mid, range_order::sorted, range_order::unique_sorted);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, {0,4}, 2, {2,3}, {2,1}, {4,2}, 4, 4, {4,6}, 6, 6, {6,3}});
    }

    SECTION("sorted -> sorted")
    {
        std::vector<key_value_pair> v{0, 2, {2,3}, {4,2}, 4, 4, {6,3}, 6};
        auto mid = v.insert(v.end(), {1, {2,1}, {2,3}, {6,1}, {6,2}});
        auto itr = do_algorithm(v, mid, range_order::sorted, range_order::sorted);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, 2, {2,3}, {2,1}, {2,3}, {4,2}, 4, 4, {6,3}, 6, {6,1}, {6,2}});
    }
}

TEST_CASE("w/ stable_sort", "[inplace_merge][stable_sort]")
{
    SECTION("uniqued -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, 2, 4, 6};
        auto mid = v.insert(v.end(), {5, 3, 1});
        auto itr = do_algorithm(v, mid, range_order::unique_sorted, range_order::uniqued);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, 2, 3, 4, 5, 6});
    }

    SECTION("uniqued -> sorted")
    {
        std::vector<key_value_pair> v{0, {2,1}, 2, 4, {4,3}, 4, 6, 6};
        auto mid = v.insert(v.end(), {3, {2,4}, {6,1}});
        auto itr = do_algorithm(v, mid, range_order::sorted, range_order::uniqued);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, {2,1}, 2, {2,4}, 3, 4, {4,3}, 4, 6, 6, {6,1}});
    }

    SECTION("no_ordered -> sorted")
    {
        std::vector<key_value_pair> v{0, 2, {2,3}, 4, {4,2}, 4, {6,7}, 6};
        auto mid = v.insert(v.end(), {5, {1,2}, 1, 4, 1, {5,2}});
        auto itr = do_algorithm(v, mid, range_order::sorted, range_order::no_ordered);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, {1,2}, 1, 1, 2, {2,3}, 4, {4,2}, 4, 4, 5, {5,2}, {6,7}, 6});
    }
}

TEST_CASE("w/ unique", "[inplace_merge][unique]")
{
    SECTION("unique_sorted -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, {2,3}, 4, {6,7}};
        auto mid = v.insert(v.end(), {1, 2, {5,2}, {6,1}});
        auto itr = do_algorithm(v, mid, range_order::unique_sorted, range_order::unique_sorted);
        REQUIRE(itr != v.end());
        v.erase(itr, v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, {2,3}, 4, {5,2}, {6,7}});
    }

    SECTION("sorted -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, 2, 4, 6};
        auto mid = v.insert(v.end(), {1, {2,3}, 2, {5,2}, 5, {6,1}});
        auto itr = do_algorithm(v, mid, range_order::unique_sorted, range_order::sorted);
        REQUIRE(itr != v.end());
        v.erase(itr, v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, 2, 4, {5,2}, 6});
    }
}

TEST_CASE("w/ stable_sort,unique", "[inplace_merge][stable_sort][unique]")
{
    SECTION("no_ordered -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, {2,3}, 4, {6,7}};
        auto mid = v.insert(v.end(), {1, {2,1}, 2, {5,2}, 5, {6,1}});
        auto itr = do_algorithm(v, mid, range_order::unique_sorted, range_order::no_ordered);
        REQUIRE(itr != v.end());
        v.erase(itr, v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, {2,3}, 4, {5,2}, {6,7}});
    }
}
