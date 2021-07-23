// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>
#include <functional>
#include <vector>

#include "flat_map/__inplace_unique_sort_merge.hpp"

using flat_map::range_order;

template <range_order Desire, typename Container>
auto do_algorithm(Container& cont, typename Container::iterator mid, range_order order)
{
    auto comp = std::less<typename Container::value_type>{};
    return flat_map::detail::inplace_unique_sort_merge(cont.begin(), mid, cont.end(),
                                                       flat_map::range_order_t<Desire>{}, order, comp, cont.get_allocator());
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

#ifndef FLAT_MAP_USE_NAIVE_IUSM
TEST_CASE("_inplace_unique_merge")
{
    using namespace flat_map::detail;

    _temporary_buffer<key_value_pair, std::allocator<key_value_pair>> buffer{32, std::allocator<key_value_pair>{}};

    SECTION("non unique")
    {
        std::vector<key_value_pair> v{{1,2}, 1, {2,3}, 2, {2,1}, 3, {3,5}, 3, 4, {5,1}, 5, 6, -1, -1, -1};
        auto itr = v.insert(v.end(), {2, {3,6}, 3, 4, {4,1}, 5, {6,1}});
        auto last = _inplace_unique_merge<range_order::sorted>(v.begin(), itr - 3, itr, v.end(), std::less<key_value_pair>{}, buffer);
        REQUIRE(last != v.end());
        v.erase(last, v.end());
        REQUIRE(v == std::vector<key_value_pair>{{1,2}, 1, {2,3}, 2, {2,1}, 2, 3, {3,5}, 3, {3,6}, 3, 4, 4, {4,1}, {5,1}, 5, 5, 6, {6,1}});
        std::cout << std::endl;
    }

    SECTION("unique")
    {
        std::vector<key_value_pair> v{{1,2}, {2,3}, 4, {5,1}, 6, -1, -1, -1};
        auto itr = v.insert(v.end(), {2, {3,6}, {4,1}, 5, {6,1}});
        auto last = _inplace_unique_merge<range_order::unique_sorted>(v.begin(), itr - 3, itr, v.end(), std::less<key_value_pair>{}, buffer);
        REQUIRE(last != v.end());
        v.erase(last, v.end());
        REQUIRE(v == std::vector<key_value_pair>{{1,2}, {2,3}, {3,6}, 4, {5,1}, 6});
    }
}

TEST_CASE("_stable_unique_sort")
{
    using namespace flat_map::detail;

    _temporary_buffer<key_value_pair, std::allocator<key_value_pair>> buffer{32, std::allocator<key_value_pair>{}};

    std::vector<key_value_pair> v{{1, 2}, 0, {7,2}, 2, 5, {6,2}, 4, {0,1}, {5,2}, 4, {4,1}, 1, {3,2}, 1, 2, {1,2}, 0, {5,3}, 6, {7,3}, {4,1}, {5,2}};
    auto itr = _stable_unique_sort<range_order::unique_sorted>(v.begin(), v.end(), std::less<key_value_pair>{}, buffer);
    REQUIRE(itr != v.end());
    v.erase(itr, v.end());
    REQUIRE(v == std::vector<key_value_pair>{0, {1,2}, 2, {3,2}, 4, 5, {6,2}, {7,2}});
}
#endif // FLAT_MAP_USE_NAIVE_IUSM

TEST_CASE("inplace_merge", "[inplace_merge]")
{
    SECTION("unique_sorted -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, 2, 4, 6};
        auto mid = v.insert(v.end(), {1, 3, 5});
        auto itr = do_algorithm<range_order::unique_sorted>(v, mid, range_order::unique_sorted);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, 2, 3, 4, 5, 6});
    }

    SECTION("unique_sorted -> sorted")
    {
        std::vector<key_value_pair> v{0, 2, {2,3}, {4,2}, 4, 4, 6, 6};
        auto mid = v.insert(v.end(), {{0,4}, {2,1}, {4,6}, {6,3}});
        auto itr = do_algorithm<range_order::sorted>(v, mid, range_order::unique_sorted);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, {0,4}, 2, {2,3}, {2,1}, {4,2}, 4, 4, {4,6}, 6, 6, {6,3}});
    }

    SECTION("sorted -> sorted")
    {
        std::vector<key_value_pair> v{0, 2, {2,3}, {4,2}, 4, 4, {6,3}, 6};
        auto mid = v.insert(v.end(), {1, {2,1}, {2,3}, {6,1}, {6,2}});
        auto itr = do_algorithm<range_order::sorted>(v, mid, range_order::sorted);
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
        auto itr = do_algorithm<range_order::unique_sorted>(v, mid, range_order::uniqued);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, 2, 3, 4, 5, 6});
    }

    SECTION("uniqued -> sorted")
    {
        std::vector<key_value_pair> v{0, {2,1}, 2, 4, {4,3}, 4, 6, 6};
        auto mid = v.insert(v.end(), {3, {2,4}, {6,1}});
        auto itr = do_algorithm<range_order::sorted>(v, mid, range_order::uniqued);
        REQUIRE(itr == v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, {2,1}, 2, {2,4}, 3, 4, {4,3}, 4, 6, 6, {6,1}});
    }

    SECTION("no_ordered -> sorted")
    {
        std::vector<key_value_pair> v{0, 2, {2,3}, 4, {4,2}, 4, {6,7}, 6};
        auto mid = v.insert(v.end(), {5, {1,2}, 1, 4, 1, {5,2}});
        auto itr = do_algorithm<range_order::sorted>(v, mid, range_order::no_ordered);
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
        auto itr = do_algorithm<range_order::unique_sorted>(v, mid, range_order::unique_sorted);
        REQUIRE(itr != v.end());
        v.erase(itr, v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, {2,3}, 4, {5,2}, {6,7}});
    }

    SECTION("sorted -> unique_sorted")
    {
        std::vector<key_value_pair> v{0, 2, 4, 6};
        auto mid = v.insert(v.end(), {1, {2,3}, 2, {5,2}, 5, {6,1}});
        auto itr = do_algorithm<range_order::unique_sorted>(v, mid, range_order::sorted);
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
        auto itr = do_algorithm<range_order::unique_sorted>(v, mid, range_order::no_ordered);
        REQUIRE(itr != v.end());
        v.erase(itr, v.end());
        REQUIRE(v == std::vector<key_value_pair>{0, 1, {2,3}, 4, {5,2}, {6,7}});
    }
}
