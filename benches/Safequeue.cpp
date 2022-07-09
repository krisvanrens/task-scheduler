#include "../source/Safequeue.hpp"

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

  state.SetBytesProcessed(static_cast<int64_t>(x.max_size()));
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

  state.SetBytesProcessed(static_cast<int64_t>(x.max_size()));
}

using ValueType = int;

BENCHMARK(BM_Construction<Safequeue<ValueType, 1>>);
BENCHMARK(BM_Construction<Safequeue<ValueType, 10>>);
BENCHMARK(BM_Construction<Safequeue<ValueType, 100>>);
BENCHMARK(BM_Construction<Safequeue<ValueType, 1'024>>);
BENCHMARK(BM_Construction<Safequeue<ValueType, Safequeue_max_size_limit>>);

BENCHMARK(BM_PushData<Safequeue<ValueType, 1>>);
BENCHMARK(BM_PushData<Safequeue<ValueType, 10>>);
BENCHMARK(BM_PushData<Safequeue<ValueType, 100>>);
BENCHMARK(BM_PushData<Safequeue<ValueType, 1'024>>);
BENCHMARK(BM_PushData<Safequeue<ValueType, Safequeue_max_size_limit>>);

BENCHMARK(BM_PopData<Safequeue<ValueType, 1>>);
BENCHMARK(BM_PopData<Safequeue<ValueType, 10>>);
BENCHMARK(BM_PopData<Safequeue<ValueType, 100>>);
BENCHMARK(BM_PopData<Safequeue<ValueType, 1'024>>);
BENCHMARK(BM_PopData<Safequeue<ValueType, Safequeue_max_size_limit>>);

BENCHMARK_MAIN();
