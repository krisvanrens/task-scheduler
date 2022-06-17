#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/Scheduler.hpp"

#include <doctest/doctest.h>

#include <cstdio>
#include <utility>

#include "../source/Task.hpp"

TEST_SUITE("Scheduler") {
  TEST_CASE("XXX") {
    using Job = Task<void()>;

    Job t1{[] { printf("t1!\n"); }};
    Job t2{[] { printf("t2!\n"); }};
    Job t3{[] { printf("t3!\n"); }};
    Job t4{[] { printf("t4!\n"); }};

    REQUIRE(t1);
    REQUIRE(t2);
    REQUIRE(t3);
    REQUIRE(t4);

    Scheduler<Job, 3> s1{1};

    CHECK(s1.schedule(std::move(t1)));
    CHECK(s1.schedule(std::move(t2)));
    CHECK(s1.schedule(std::move(t3)));

    CHECK(!t1);
    CHECK(!t2);
    CHECK(!t3);

    CHECK(!s1.schedule(std::move(t4)));
    CHECK(t4);

    CHECK_THROWS_AS((Scheduler<Job, 3>{100}), std::overflow_error);
  }
} // TEST_SUITE
