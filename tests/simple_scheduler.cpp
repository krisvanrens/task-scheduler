#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/simple_scheduler.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>
#include <utility>

#include "../source/task.hpp"

using namespace ts;
using namespace std::chrono_literals;

const auto NUM_CORES = std::thread::hardware_concurrency();

TEST_SUITE("simple_scheduler") {
  TEST_CASE("Construction") {
    simple_scheduler<1>   s1{1};
    simple_scheduler<10>  s2{1};
    simple_scheduler<100> s3{NUM_CORES};
  }

  TEST_CASE("Construction (failure cases)") {
    CHECK_THROWS_AS((simple_scheduler<10>{0}), std::underflow_error);
    CHECK_THROWS_AS((simple_scheduler<10>{1024}), std::overflow_error);
  }

  TEST_CASE("Getting the number of executors") {
    CHECK(simple_scheduler<10>{1}.num_executors() == 1);
    CHECK(simple_scheduler<10>{NUM_CORES}.num_executors() == NUM_CORES);
  }

  TEST_CASE("Schedule jobs" * doctest::skip(NUM_CORES < 2)) {
    std::array<bool, 4> call_status = {};

    task<void()> t0{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[0] = true;
    }};

    task<void()> t1{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[1] = true;
    }};

    task<void()> t2{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[2] = true;
    }};

    task<void()> t3{[&] {
      std::this_thread::sleep_for(20ms);
      call_status[3] = true;
    }};

    REQUIRE(t0);
    REQUIRE(t1);
    REQUIRE(t2);
    REQUIRE(t3);

    simple_scheduler<2> s{2};

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

    simple_scheduler<2> s{2};

    REQUIRE(s.schedule([&] { callback(); }));
    REQUIRE(s.schedule([&] { callback(); }));
    REQUIRE(s.schedule([&] { callback(); }));
    REQUIRE(s.schedule([&] { callback(); }));

    // Wait until (hopefully) all tasks are flushed.
    std::this_thread::sleep_for(100ms);

    CHECK(count == 4);
  }

  TEST_CASE("Flush scheduler" * doctest::timeout(1)) {
    const auto time_start = std::chrono::system_clock::now();

    {
      simple_scheduler<3> s{1};

      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(100ms); }));

      // Wait until first task is taken on.
      std::this_thread::sleep_for(50ms);

      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(1s); }));
      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(1s); }));
      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(1s); }));

      s.flush();
    }

    const auto time_end = std::chrono::system_clock::now();

    CHECK((time_end - time_start) < 200ms);
  }

  TEST_CASE("Test graceful exception handling" * doctest::timeout(1)) {
    CHECK_NOTHROW([] {
      simple_scheduler<3> s{1};
      (void)s.schedule([] { throw std::exception{}; });
    });
  }

  TEST_CASE("Test exception handling and checking" * doctest::timeout(1)) {
    simple_scheduler<4> s{1};

    auto completion0 = s.schedule([] {});
    auto completion1 = s.schedule([] { throw std::exception{}; });
    auto completion2 = s.schedule([] { throw std::logic_error{"logic"}; });
    auto completion3 = s.schedule([] { throw std::runtime_error{"runtime"}; });

    // Wait until (hopefully) all tasks are flushed.
    std::this_thread::sleep_for(100ms);

    REQUIRE(completion0);
    REQUIRE(completion1);
    REQUIRE(completion2);
    REQUIRE(completion3);

    CHECK_FALSE(completion0->exception().has_value());

    REQUIRE(completion1->exception().has_value());
    REQUIRE(completion2->exception().has_value());
    REQUIRE(completion3->exception().has_value());

    try {
      std::rethrow_exception(*completion2->exception());
    } catch (const std::exception& error) {
      CHECK(std::string{error.what()} == "logic");
    }

    try {
      std::rethrow_exception(*completion3->exception());
    } catch (const std::exception& error) {
      CHECK(std::string{error.what()} == "runtime");
    }
  }

} // TEST_SUITE
