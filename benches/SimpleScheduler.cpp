#include "../source/SimpleScheduler.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>

using Scheduler = SimpleScheduler<100>;

static void BM_Construction(benchmark::State& state) {
  for (auto _ : state) {
    Scheduler x{static_cast<std::size_t>(state.range(0))};
  }
}

BENCHMARK(BM_Construction)->RangeMultiplier(2)->Range(1, 16);

BENCHMARK_MAIN();
