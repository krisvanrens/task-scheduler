#include "../source/safe_queue.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>

template<typename Queue>
void BM_Construction(benchmark::State& state) {
  for (auto _ : state) {
    Queue x;
  }
}

template<typename Queue>
void BM_PushData(benchmark::State& state) {
  Queue x;

  for (auto _ : state) {
    for (std::size_t i = 0; i < x.max_size(); i++) {
      [[maybe_unused]] auto result = x.push(42);
    }
  }

  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(x.max_size()));
}

template<typename Queue>
void BM_PopData(benchmark::State& state) {
  Queue x;

  for (auto _ : state) {
    state.PauseTiming();

    for (std::size_t i = 0; i < x.max_size(); i++) {
      [[maybe_unused]] auto result = x.push(42);
    }

    state.ResumeTiming();

    for (std::size_t i = 0; i < x.max_size(); i++) {
      [[maybe_unused]] auto result = x.pop();
    }
  }

  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(x.max_size()));
}

using namespace ts;

BENCHMARK(BM_Construction<safe_queue<int, 1>>);
BENCHMARK(BM_Construction<safe_queue<int, 10>>);
BENCHMARK(BM_Construction<safe_queue<int, 100>>);
BENCHMARK(BM_Construction<safe_queue<int, 1'024>>);
BENCHMARK(BM_Construction<safe_queue<int, safe_queue_max_size_limit>>);

BENCHMARK(BM_PushData<safe_queue<int, 1>>);
BENCHMARK(BM_PushData<safe_queue<int, 10>>);
BENCHMARK(BM_PushData<safe_queue<int, 100>>);
BENCHMARK(BM_PushData<safe_queue<int, 1'024>>);
BENCHMARK(BM_PushData<safe_queue<int, safe_queue_max_size_limit>>);

BENCHMARK(BM_PopData<safe_queue<int, 1>>);
BENCHMARK(BM_PopData<safe_queue<int, 10>>);
BENCHMARK(BM_PopData<safe_queue<int, 100>>);
BENCHMARK(BM_PopData<safe_queue<int, 1'024>>);
BENCHMARK(BM_PopData<safe_queue<int, safe_queue_max_size_limit>>);

BENCHMARK_MAIN();
