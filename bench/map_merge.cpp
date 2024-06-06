#include <benchmark/benchmark.h>
#include <deque>
#include <flat_map/flat_map.hpp>
#include <map>
#include <random>
#include <unordered_map>
#include <vector>

inline constexpr auto k_factor = 100;

static std::mt19937 rng_state{};

inline constexpr std::pair<int64_t, int64_t> range{4, 1 << 16};

static std::vector<std::pair<int, int>> const v = []
{
    std::vector<std::pair<int, int>> v;
    v.resize(range.second * 2);
    for (auto& [k, v] : v)
    {
        k = std::uniform_int_distribution<int>{}(rng_state);
        v = std::uniform_int_distribution<int>{}(rng_state);
    }
    return v;
}();

template <typename C, int k_factor>
static void BM_merge(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        state.counters["k_factor"] = k_factor;

        auto off = std::uniform_int_distribution<int>{0, range.second}(rng_state);
        C orig(std::next(v.begin(), off), std::next(v.begin(), off + state.range(0)));

        off = std::uniform_int_distribution<int>{0, range.second}(rng_state);
        C src(std::next(v.begin(), off), std::next(v.begin(), off + state.range(1)));

        for (auto i = 1; i < k_factor; ++i)
        {
            auto dst = orig;
            benchmark::ClobberMemory();

            state.ResumeTiming();
            dst.merge(src);
            benchmark::ClobberMemory();
            state.PauseTiming();
        }

        state.ResumeTiming();
        orig.merge(src);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_merge<std::map<int, int>, 1>)->Ranges({range, range});
BENCHMARK(BM_merge<std::unordered_map<int, int>, 1>)->Ranges({range, range});
BENCHMARK(BM_merge<flat_map::flat_map<int, int>, k_factor>)->Ranges({range, range});
BENCHMARK(BM_merge<flat_map::flat_map<int, int, std::less<int>, std::deque<std::pair<int, int>>>, k_factor>)->Ranges({range, range});

BENCHMARK_MAIN();
