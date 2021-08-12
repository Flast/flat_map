// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <benchmark/benchmark.h>
#include <deque>
#include <flat_map/__inplace_unique_sort_merge.hpp>
#include <random>
#include <vector>

static std::mt19937 rng_state{};

template <typename C>
static void BM_inplace_merge(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        C v;
        v.resize(state.range(0));
        for (auto& e : v)
        {
            e = std::uniform_int_distribution<int>{}(rng_state);
        }
        auto m = v.begin() + v.size() / 2;
        std::stable_sort(v.begin(), m, std::less<int>{});
        std::stable_sort(m, v.end(), std::less<int>{});
        state.ResumeTiming();

        flat_map::detail::_temporary_buffer buffer(std::distance(v.begin(), m), std::allocator<int>{});
        flat_map::detail::_inplace_unique_merge<flat_map::range_order::sorted>(v.begin(), m, m, v.end(), std::less<int>{}, buffer);
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_inplace_merge, std::vector<int>)->Range(64, 1 << 18);
BENCHMARK_TEMPLATE(BM_inplace_merge, std::deque<int>)->Range(64, 1 << 18);

template <typename C>
static void BM_std_inplace_merge(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        C v;
        v.resize(state.range(0));
        for (auto& e : v)
        {
            e = std::uniform_int_distribution<int>{}(rng_state);
        }
        auto m = v.begin() + v.size() / 2;
        std::stable_sort(v.begin(), m, std::less<int>{});
        std::stable_sort(m, v.end(), std::less<int>{});
        state.ResumeTiming();

        std::inplace_merge(v.begin(), m, v.end(), std::less<int>{});
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_std_inplace_merge, std::vector<int>)->Range(64, 1 << 18);
BENCHMARK_TEMPLATE(BM_std_inplace_merge, std::deque<int>)->Range(64, 1 << 18);

template <typename C>
static void BM_inplace_merge_unique(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        C v;
        v.resize(state.range(0));
        for (auto& e : v)
        {
            e = std::uniform_int_distribution<int>{}(rng_state);
        }
        auto m = v.begin() + v.size() / 2;
        std::stable_sort(v.begin(), m, std::less<int>{});
        std::stable_sort(m, v.end(), std::less<int>{});
        auto l1 = std::unique(v.begin(), m, std::equal_to<int>{});
        auto l2 = std::unique(m, v.end(), std::equal_to<int>{});
        auto end = std::move(m, l2, l1);
        state.ResumeTiming();

        flat_map::detail::_temporary_buffer buffer(std::distance(v.begin(), l1), std::allocator<int>{});
        flat_map::detail::_inplace_unique_merge<flat_map::range_order::unique_sorted>(v.begin(), l1, l1, end, std::less<int>{}, buffer);
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_inplace_merge, std::vector<int>)->Range(64, 1 << 18);
BENCHMARK_TEMPLATE(BM_inplace_merge, std::deque<int>)->Range(64, 1 << 18);

template <typename C>
static void BM_std_inplace_merge_unique(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        C v;
        v.resize(state.range(0));
        for (auto& e : v)
        {
            e = std::uniform_int_distribution<int>{}(rng_state);
        }
        auto m = v.begin() + v.size() / 2;
        std::stable_sort(v.begin(), m, std::less<int>{});
        std::stable_sort(m, v.end(), std::less<int>{});
        auto l1 = std::unique(v.begin(), m, std::equal_to<int>{});
        auto l2 = std::unique(m, v.end(), std::equal_to<int>{});
        auto end = std::move(m, l2, l1);
        state.ResumeTiming();

        std::inplace_merge(v.begin(), l1, end, std::less<int>{});
        std::unique(v.begin(), end, std::equal_to<int>{});
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_std_inplace_merge_unique, std::vector<int>)->Range(64, 1 << 18);
BENCHMARK_TEMPLATE(BM_std_inplace_merge_unique, std::deque<int>)->Range(64, 1 << 18);

BENCHMARK_MAIN();
