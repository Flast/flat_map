#include <benchmark/benchmark.h>
#include <deque>
#include <flat_map/flat_map.hpp>
#include <map>
#include <random>
#include <unordered_map>
#include <vector>

static std::mt19937 rng_state{};

template <typename C>
static void BM_merge(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        C src, dst;

        std::vector<std::pair<int, int>> v(state.range(0));
        for (auto& [k, v] : v)
        {
            k = std::uniform_int_distribution<int>{}(rng_state);
            v = std::uniform_int_distribution<int>{}(rng_state);
        }
        dst = C(v.begin(), v.end());

        v.resize(state.range(1));
        for (auto& [k, v] : v)
        {
            k = std::uniform_int_distribution<int>{}(rng_state);
            v = std::uniform_int_distribution<int>{}(rng_state);
        }
        src = C(v.begin(), v.end());
        state.ResumeTiming();

        dst.merge(src);
        benchmark::DoNotOptimize(dst.begin());
        benchmark::ClobberMemory();
    }
}
BENCHMARK_TEMPLATE(BM_merge, std::map<int, int>)->Ranges({{4, 1 << 16}, {4, 1 << 16}});
BENCHMARK_TEMPLATE(BM_merge, std::unordered_map<int, int>)->Ranges({{4, 1 << 16}, {4, 1 << 16}});
BENCHMARK_TEMPLATE(BM_merge, flat_map::flat_map<int, int>)->Ranges({{4, 1 << 16}, {4, 1 << 16}});
BENCHMARK_TEMPLATE(BM_merge, flat_map::flat_map<int, int, std::less<int>, std::deque<std::pair<int, int>>>)->Ranges({{4, 1 << 16}, {4, 1 << 16}});

BENCHMARK_MAIN();
