#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/SimpleScheduler.hpp"

#include <doctest/doctest.h>

#include <cstdio>
#include <stdexcept>
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

  TEST_CASE("Move construction") {
    SimpleScheduler<10> x{10};
    SimpleScheduler<10> y{std::move(x)};
  }

  TEST_CASE("Move construction") {
    SimpleScheduler<10> x{10};
    SimpleScheduler<10> y{1};

    y = std::move(x);
  }

  TEST_CASE("Getting the number of executors") {
    CHECK(SimpleScheduler<10>{1}.num_executors() == 1);
    CHECK(SimpleScheduler<10>{2}.num_executors() == 2);
    CHECK(SimpleScheduler<10>{10}.num_executors() == 10);
  }

#if 0
  TEST_CASE("XXX") {
    SimpleScheduler<3>::Job t1{[] { printf("t1!\n"); return 42; }};
    SimpleScheduler<3>::Job t2{[] { printf("t2!\n"); return 42; }};
    SimpleScheduler<3>::Job t3{[] { printf("t3!\n"); return 42; }};
    SimpleScheduler<3>::Job t4{[] { printf("t4!\n"); return 42; }};
    SimpleScheduler<3>::Job t5{[] { printf("t5!\n"); return 42; }};
    SimpleScheduler<3>::Job t6{[] { printf("t6!\n"); return 42; }};
    SimpleScheduler<3>::Job t7{[] { printf("t7!\n"); return 42; }};

    REQUIRE(t1);
    REQUIRE(t2);
    REQUIRE(t3);
    REQUIRE(t4);

    SimpleScheduler<3> s1{2};

    CHECK(s1.schedule(std::move(t1)));
    CHECK(s1.schedule(std::move(t2)));
    CHECK(s1.schedule(std::move(t3)));
    CHECK(s1.schedule(std::move(t4)));
    CHECK(s1.schedule(std::move(t5)));
    CHECK(s1.schedule(std::move(t6)));

    CHECK(!t1);
    CHECK(!t2);
    CHECK(!t3);
    CHECK(!t4);
    CHECK(!t5);
    CHECK(!t6);

    CHECK(!s1.schedule(std::move(t7)));
    CHECK(t7);
  }
#endif

} // TEST_SUITE
