// Copyright (c) 2023 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <catch2/catch.hpp>

#include "flat_map/__tuple.hpp"

struct unique_type
{
    unsigned value = 0;

    constexpr unique_type() = default;
    constexpr unique_type(unsigned value) : value{value} {}
    constexpr unique_type(unique_type const& other) = default;
    constexpr unique_type(unique_type&& other) : value{other.value}
    {
        other.value = 0;
    }

    constexpr unique_type& operator=(unique_type const& other) = default;
    constexpr unique_type& operator=(unique_type&& other)
    {
        value = other.value;
        other.value = 0;
        return *this;
    }
};

using tuple = flat_map::detail::tuple<int, unique_type>;
using rtuple = flat_map::detail::tuple<int&, unique_type&>;

TEST_CASE("construction", "[construction]")
{
    SECTION("standard type traits")
    {
        static_assert(std::is_same_v<std::tuple_element_t<0, tuple>, int>);
        static_assert(std::is_same_v<std::tuple_element_t<1, tuple>, unique_type>);
        static_assert(std::tuple_size_v<tuple> == 2);
    }

    SECTION("default construction")
    {
        [[maybe_unused]] tuple st;
    }

    SECTION("copy construction")
    {
        tuple st{42, unique_type{0xdeadbeefu}};

        tuple copy = st;

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(st).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("copy construction from std::tuple")
    {
        std::tuple t{42, unique_type{0xdeadbeefu}};

        tuple copy = t;

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(t).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("move construction")
    {
        tuple st{42, unique_type{0xdeadbeefu}};

        tuple move = std::move(st);

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(st).value == 42);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }

    SECTION("move construction from std::tuple")
    {
        std::tuple t{42, unique_type{0xdeadbeefu}};

        [[maybe_unused]] tuple move = std::move(t);

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(t).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }
}

TEST_CASE("assignment", "[assignment]")
{
    SECTION("copy assignment")
    {
        tuple st{42, unique_type{0xdeadbeefu}};
        tuple copy;

        copy = st;

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(st).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("copy assignment from std::tuple")
    {
        std::tuple t{42, unique_type{0xdeadbeefu}};
        tuple copy;

        copy = t;

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(t).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("move assignment")
    {
        tuple st{42, unique_type{0xdeadbeefu}};
        tuple move;

        move = std::move(st);

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(st).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }

    SECTION("move assignment from std::tuple")
    {
        std::tuple t{42, unique_type{0xdeadbeefu}};
        tuple move;

        move = std::move(t);

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(t).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }
}

TEST_CASE("assignment with reference", "[assignment]")
{
    SECTION("copy assignment")
    {
        tuple st{42, unique_type{0xdeadbeefu}};

        int i = 72;
        unique_type u;
        rtuple copy{i, u};

        copy = st;

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(st).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("copy assignment from std::tuple")
    {
        std::tuple t{42, unique_type{0xdeadbeefu}};

        int i = 72;
        unique_type u;
        rtuple copy{i, u};

        copy = t;

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(t).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("move assignment")
    {
        tuple st{42, unique_type{0xdeadbeefu}};

        int i = 72;
        unique_type u;
        tuple move{i, u};

        move = std::move(st);

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(st).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }

    SECTION("move assignment from std::tuple")
    {
        std::tuple t{42, unique_type{0xdeadbeefu}};

        int i = 72;
        unique_type u;
        tuple move{i, u};

        move = std::move(t);

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(t).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }

    SECTION("copy assignment by reference")
    {
        int i = 42;
        unique_type u{0xdeadbeefu};
        rtuple st{i, u};

        tuple copy{72, unique_type{}};

        copy = st;

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(st).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("copy assignment from std::tuple by reference")
    {
        int i = 42;
        unique_type u{0xdeadbeefu};
        rtuple t{i, u};

        std::tuple copy{72, unique_type{}};

        copy = t;

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(copy) == 42);
        REQUIRE(std::get<1>(t).value == 0xdeadbeefu);
        REQUIRE(std::get<1>(copy).value == 0xdeadbeefu);
    }

    SECTION("move assignment by reference")
    {
        int i = 42;
        unique_type u{0xdeadbeefu};
        tuple move{i, u};

        tuple st{72, unique_type{}};

        move = std::move(st);

        REQUIRE(std::get<0>(st) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(st).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }

    SECTION("move assignment from std::tuple by reference")
    {
        int i = 42;
        unique_type u{0xdeadbeefu};
        tuple move{i, u};

        std::tuple t{72, unique_type{}};

        move = std::move(t);

        REQUIRE(std::get<0>(t) == 42);
        REQUIRE(std::get<0>(move) == 42);
        REQUIRE(std::get<1>(t).value == 0);
        REQUIRE(std::get<1>(move).value == 0xdeadbeefu);
    }
}
