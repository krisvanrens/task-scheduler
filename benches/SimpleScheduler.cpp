#include "../source/SimpleScheduler.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <thread>

static constexpr std::size_t QUEUE_LENGTH = 100;

using Scheduler = SimpleScheduler<QUEUE_LENGTH>;

static void BM_Construction(benchmark::State& state) {
  for (auto _ : state) {
    Scheduler x{static_cast<std::size_t>(state.range(0))};
  }
}

static void BM_ScheduleWork(benchmark::State& state) {
  Scheduler x{static_cast<std::size_t>(state.range(0))};
  const auto schedule_tasks = [&x]{
    for (unsigned int i = 0; i < QUEUE_LENGTH; i++) {
      [[maybe_unused]] auto result = x.schedule([]{});
    }
  };

  for (auto _ : state) {
    schedule_tasks();
  }
}

BENCHMARK(BM_Construction)->RangeMultiplier(2)->Range(1, std::thread::hardware_concurrency());
BENCHMARK(BM_ScheduleWork)->RangeMultiplier(2)->Range(1, std::thread::hardware_concurrency());

BENCHMARK_MAIN();
