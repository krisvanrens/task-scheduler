#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/SimpleScheduler.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include "../source/Task.hpp"

using namespace ts;
using namespace std::chrono_literals;

const auto NUM_CORES = std::thread::hardware_concurrency();

TEST_SUITE("SimpleScheduler") {
  TEST_CASE("Construction") {
    SimpleScheduler<1>   s1{1};
    SimpleScheduler<10>  s2{1};
    SimpleScheduler<100> s3{NUM_CORES};
  }

  TEST_CASE("Construction (failure cases)") {
    CHECK_THROWS_AS((SimpleScheduler<10>{0}), std::underflow_error);
    CHECK_THROWS_AS((SimpleScheduler<10>{1024}), std::overflow_error);
  }

  TEST_CASE("Getting the number of executors") {
    CHECK(SimpleScheduler<10>{1}.num_executors() == 1);
    CHECK(SimpleScheduler<10>{NUM_CORES}.num_executors() == NUM_CORES);
  }

  TEST_CASE("Schedule jobs" * doctest::skip(NUM_CORES < 2)) {
    std::array<bool, 4> call_status = {};

    Task<void()> t0{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[0] = true;
    }};

    Task<void()> t1{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[1] = true;
    }};

    Task<void()> t2{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[2] = true;
    }};

    Task<void()> t3{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[3] = true;
    }};

    REQUIRE(t0);
    REQUIRE(t1);
    REQUIRE(t2);
    REQUIRE(t3);

    SimpleScheduler<2> s{2};

    const auto check_schedule = [&](auto&& task) {
      auto completion = s.schedule(std::move(task));

      CHECK(completion);
      CHECK_FALSE(task);
      CHECK_FALSE(*completion);

      return completion.value();
    };

    auto completion0 = check_schedule(std::move(t0));
    auto completion1 = check_schedule(std::move(t1));
    auto completion2 = check_schedule(std::move(t2));
    auto completion3 = check_schedule(std::move(t3));

    // Wait until (hopefully) all tasks are flushed.
    std::this_thread::sleep_for(100ms);

    CHECK(completion0);
    CHECK(completion1);
    CHECK(completion2);
    CHECK(completion3);

    CHECK(std::all_of(call_status.begin(), call_status.end(), [](bool status) { return status; }));
  }

  TEST_CASE("Schedule a local callback" * doctest::skip(NUM_CORES < 2)) {
    std::atomic<unsigned int> count = 0;

    const auto callback = [&] { count++; };

    SimpleScheduler<2> s{2};

    REQUIRE(s.schedule([&] { callback(); }));
    REQUIRE(s.schedule([&] { callback(); }));
    REQUIRE(s.schedule([&] { callback(); }));
    REQUIRE(s.schedule([&] { callback(); }));

    // Wait until (hopefully) all tasks are flushed.
    std::this_thread::sleep_for(100ms);

    CHECK(count == 4);
  }

  TEST_CASE("Flush scheduler" * doctest::timeout(1)) {
    const auto timeStart = std::chrono::system_clock::now();

    {
      SimpleScheduler<3> s{1};

      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(100ms); }));

      // Wait until first task is taken on.
      std::this_thread::sleep_for(50ms);

      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(1s); }));
      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(1s); }));
      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(1s); }));

      s.flush();
    }

    const auto timeEnd = std::chrono::system_clock::now();

    CHECK((timeEnd - timeStart) < 200ms);
  }

} // TEST_SUITE
