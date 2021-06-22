#include <benchmark/benchmark.h>
#include <deque>
#include <flat_map/flat_map.hpp>
#include <map>
#include <random>
#include <unordered_map>
#include <vector>

static std::mt19937 rng_state{};

template <typename C>
static void BM_range_insertion(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<std::pair<int, int>> v(state.range(0));
        for (auto& [k, v] : v)
        {
            k = std::uniform_int_distribution<int>{}(rng_state);
            v = std::uniform_int_distribution<int>{}(rng_state);
        }
        state.ResumeTiming();

        C fm;
        fm.insert(v.begin(), v.end());
        benchmark::DoNotOptimize(fm.begin());
        benchmark::ClobberMemory();
    }
}
BENCHMARK_TEMPLATE(BM_range_insertion, std::map<int, int>)->Range(4, 1 << 18);
BENCHMARK_TEMPLATE(BM_range_insertion, std::unordered_map<int, int>)->Range(4, 1 << 18);
BENCHMARK_TEMPLATE(BM_range_insertion, flat_map::flat_map<int, int>)->Range(4, 1 << 18);
BENCHMARK_TEMPLATE(BM_range_insertion, flat_map::flat_map<int, int, std::less<int>, std::deque<std::pair<int, int>>>)->Range(4, 1 << 18);

BENCHMARK_MAIN();
