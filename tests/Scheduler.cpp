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
    Job t5{[] { printf("t5!\n"); }};
    Job t6{[] { printf("t6!\n"); }};
    Job t7{[] { printf("t7!\n"); }};

    REQUIRE(t1);
    REQUIRE(t2);
    REQUIRE(t3);
    REQUIRE(t4);

    Scheduler<Job, 3> s1{2};

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

    CHECK_THROWS_AS((Scheduler<Job, 3>{100}), std::overflow_error);
  }

} // TEST_SUITE
