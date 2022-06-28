#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/SimpleScheduler.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

#include "../source/Task.hpp"

TEST_SUITE("SimpleScheduler") {
  TEST_CASE("Construction") {
    SimpleScheduler<1>   s1{1};
    SimpleScheduler<10>  s2{1};
    SimpleScheduler<100> s3{10};
  }

  TEST_CASE("Construction (failure cases)") {
    CHECK_THROWS_AS((SimpleScheduler<10>{0}), std::underflow_error);
    CHECK_THROWS_AS((SimpleScheduler<10>{100}), std::overflow_error);
  }

  TEST_CASE("Getting the number of executors") {
    CHECK(SimpleScheduler<10>{1}.num_executors() == 1);
    CHECK(SimpleScheduler<10>{2}.num_executors() == 2);
    CHECK(SimpleScheduler<10>{10}.num_executors() == 10);
  }

  TEST_CASE("Schedule jobs") {
    std::array<bool, 6> call_status = {};

    Task<void()> t0{[&] { call_status[0] = true; }};
    Task<void()> t1{[&] { call_status[1] = true; }};
    Task<void()> t2{[&] { call_status[2] = true; }};
    Task<void()> t3{[&] { call_status[3] = true; }};
    Task<void()> t4{[&] { call_status[4] = true; }};
    Task<void()> t5{[&] { call_status[5] = true; }};
    Task<void()> t6{[] {}};

    REQUIRE(t0);
    REQUIRE(t1);
    REQUIRE(t2);
    REQUIRE(t3);
    REQUIRE(t4);
    REQUIRE(t5);

    SimpleScheduler<3> s{2};

    CHECK(s.schedule(std::move(t0)));
    CHECK(s.schedule(std::move(t1)));
    CHECK(s.schedule(std::move(t2)));
    CHECK(s.schedule(std::move(t3)));
    CHECK(s.schedule(std::move(t4)));
    CHECK(s.schedule(std::move(t5)));

    CHECK(!t0);
    CHECK(!t1);
    CHECK(!t2);
    CHECK(!t3);
    CHECK(!t4);
    CHECK(!t5);

    CHECK(!s.schedule(std::move(t6)));
    CHECK(t6);

    // Wait until (hopefully) all tasks are flushed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CHECK(std::all_of(call_status.begin(), call_status.end(), [](bool status) { return status; }));
  }


  TEST_CASE("Flush scheduler") {
    const auto timeStart = std::chrono::system_clock::now();

    {
      SimpleScheduler<3> s{1};

      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }));

      // Wait until first task is taken on.
      std::this_thread::sleep_for(std::chrono::milliseconds(50));


      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(std::chrono::seconds(1)); }));
      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(std::chrono::seconds(1)); }));
      REQUIRE(s.schedule([&] { std::this_thread::sleep_for(std::chrono::seconds(1)); }));

      s.flush();
    }

    const auto timeEnd = std::chrono::system_clock::now();

    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() < 200);
  }

} // TEST_SUITE
