#include "../source/Safequeue.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>

static constexpr std::size_t QUEUE_LENGTH = 1024;

using Queue = Safequeue<unsigned int, QUEUE_LENGTH>;

static void BM_Construction(benchmark::State& state) {
  for (auto _ : state) {
    Queue x;
  }
}

static void BM_Pushdata(benchmark::State& state) {
  Queue x;

  for (auto _ : state) {
    for (unsigned int i = 0; i < QUEUE_LENGTH; i++) {
      [[maybe_unused]] auto result = x.push(i);
    }
  }
}

static void BM_Popdata(benchmark::State& state) {
  Queue x;

  for (auto _ : state) {
    state.PauseTiming();

    for (unsigned int i = 0; i < QUEUE_LENGTH; i++) {
      [[maybe_unused]] auto result = x.push(i);
    }

    state.ResumeTiming();

    for (unsigned int i = 0; i < QUEUE_LENGTH; i++) {
      [[maybe_unused]] auto result = x.pop();
    }
  }
}

BENCHMARK(BM_Construction);
BENCHMARK(BM_Pushdata);
BENCHMARK(BM_Popdata);

BENCHMARK_MAIN();
