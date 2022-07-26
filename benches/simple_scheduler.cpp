#include "../source/simple_scheduler.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <thread>

using namespace ts;

static constexpr std::size_t QUEUE_LENGTH = 100;

using test_scheduler = simple_scheduler<QUEUE_LENGTH>;

static void BM_Construction(benchmark::State& state) {
  const auto num_executors = static_cast<std::size_t>(state.range(0));

  for (auto _ : state) {
    benchmark::DoNotOptimize(test_scheduler{num_executors});
  }
}

static void BM_ScheduleWork(benchmark::State& state) {
  const auto num_executors = static_cast<std::size_t>(state.range(0));

  test_scheduler s{num_executors};

  for (auto _ : state) {
    for (unsigned int i = 0; i < QUEUE_LENGTH; i++) {
      benchmark::DoNotOptimize(s.schedule([]{}));
    }
  }
}

BENCHMARK(BM_Construction)->RangeMultiplier(2)->Range(1, std::thread::hardware_concurrency());
BENCHMARK(BM_ScheduleWork)->RangeMultiplier(2)->Range(1, std::thread::hardware_concurrency());

BENCHMARK_MAIN();
