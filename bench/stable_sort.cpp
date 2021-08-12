// Copyright (c) 2021 Kohei Takahashi
// This software is released under the MIT License, see LICENSE.

#include <benchmark/benchmark.h>
#include <deque>
#include <flat_map/__inplace_unique_sort_merge.hpp>
#include <random>
#include <vector>

static std::mt19937 rng_state{};

template <typename C>
static void BM_insertion_sort(benchmark::State& state)
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
        state.ResumeTiming();

        flat_map::detail::_insertion_unique_sort<flat_map::range_order::sorted>(v.begin(), v.end(), std::less<int>{});
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_insertion_sort, std::vector<int>)->DenseRange(8, 128, 16);
BENCHMARK_TEMPLATE(BM_insertion_sort, std::deque<int>)->DenseRange(8, 128, 16);

template <typename C>
static void BM_small_std_stable_sort(benchmark::State& state)
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
        state.ResumeTiming();

        std::stable_sort(v.begin(), v.end(), std::less<int>{});
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_small_std_stable_sort, std::vector<int>)->DenseRange(8, 128, 16);
BENCHMARK_TEMPLATE(BM_small_std_stable_sort, std::deque<int>)->DenseRange(8, 128, 16);

template <typename C>
static void BM_insertion_sort_unique(benchmark::State& state)
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
        state.ResumeTiming();

        flat_map::detail::_insertion_unique_sort<flat_map::range_order::unique_sorted>(v.begin(), v.end(), std::less<int>{});
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_insertion_sort_unique, std::vector<int>)->DenseRange(8, 128, 16);
BENCHMARK_TEMPLATE(BM_insertion_sort_unique, std::deque<int>)->DenseRange(8, 128, 16);

template <typename C>
static void BM_small_std_stable_sort_unique(benchmark::State& state)
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
        state.ResumeTiming();

        std::stable_sort(v.begin(), v.end(), std::less<int>{});
        std::unique(v.begin(), v.end(), std::equal_to<int>{});
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_small_std_stable_sort_unique, std::vector<int>)->DenseRange(8, 128, 16);
BENCHMARK_TEMPLATE(BM_small_std_stable_sort_unique, std::deque<int>)->DenseRange(8, 128, 16);

template <typename C>
static void BM_stable_sort(benchmark::State& state)
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
        state.ResumeTiming();

        flat_map::detail::_temporary_buffer buffer((state.range(0) + 1) / 2, std::allocator<int>{});
        flat_map::detail::_stable_unique_sort<flat_map::range_order::sorted>(v.begin(), v.end(), std::less<int>{}, buffer);
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_stable_sort, std::vector<int>)->Range(32, 1 << 18);
BENCHMARK_TEMPLATE(BM_stable_sort, std::deque<int>)->Range(32, 1 << 18);

template <typename C> static void BM_large_std_stable_sort(benchmark::State& state) { BM_small_std_stable_sort<C>(state); }
BENCHMARK_TEMPLATE(BM_large_std_stable_sort, std::vector<int>)->Range(32, 1 << 18);
BENCHMARK_TEMPLATE(BM_large_std_stable_sort, std::deque<int>)->Range(32, 1 << 18);

template <typename C>
static void BM_stable_sort_unique(benchmark::State& state)
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
        state.ResumeTiming();

        flat_map::detail::_temporary_buffer buffer((state.range(0) + 1) / 2, std::allocator<int>{});
        flat_map::detail::_stable_unique_sort<flat_map::range_order::unique_sorted>(v.begin(), v.end(), std::less<int>{}, buffer);
        benchmark::DoNotOptimize(v.begin());
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_stable_sort_unique, std::vector<int>)->Range(32, 1 << 18);
BENCHMARK_TEMPLATE(BM_stable_sort_unique, std::deque<int>)->Range(32, 1 << 18);

template <typename C> static void BM_large_std_stable_sort_unique(benchmark::State& state) { BM_small_std_stable_sort_unique<C>(state); }
BENCHMARK_TEMPLATE(BM_large_std_stable_sort_unique, std::vector<int>)->Range(32, 1 << 18);
BENCHMARK_TEMPLATE(BM_large_std_stable_sort_unique, std::deque<int>)->Range(32, 1 << 18);

BENCHMARK_MAIN();
