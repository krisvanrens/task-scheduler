#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "tracer.hpp"

#include <doctest/doctest.h>

#include <utility>

TEST_SUITE("tracer") {
  TEST_CASE("Construction") {
    tracer t;

    REQUIRE(t.counts().ctor == 1);
    REQUIRE(t.counts().dtor == 0);
    REQUIRE(t.counts().copy_ctor == 0);
    REQUIRE(t.counts().copy_assign == 0);
    REQUIRE(t.counts().move_ctor == 0);
    REQUIRE(t.counts().move_assign == 0);
  }

  TEST_CASE("Copy construction") {
    tracer t1;

    tracer t2{t1};

    REQUIRE(t1.counts().ctor == 1);
    REQUIRE(t1.counts().dtor == 0);
    REQUIRE(t1.counts().copy_ctor == 0);
    REQUIRE(t1.counts().copy_assign == 0);
    REQUIRE(t1.counts().move_ctor == 0);
    REQUIRE(t1.counts().move_assign == 0);

    REQUIRE(t2.counts().ctor == 1);
    REQUIRE(t2.counts().dtor == 0);
    REQUIRE(t2.counts().copy_ctor == 1);
    REQUIRE(t2.counts().copy_assign == 0);
    REQUIRE(t2.counts().move_ctor == 0);
    REQUIRE(t2.counts().move_assign == 0);
  }

  TEST_CASE("Copy assignment") {
    tracer t1, t2;

    t2 = t1;

    REQUIRE(t1.counts().ctor == 1);
    REQUIRE(t1.counts().dtor == 0);
    REQUIRE(t1.counts().copy_ctor == 0);
    REQUIRE(t1.counts().copy_assign == 0);
    REQUIRE(t1.counts().move_ctor == 0);
    REQUIRE(t1.counts().move_assign == 0);

    REQUIRE(t2.counts().ctor == 1);
    REQUIRE(t2.counts().dtor == 0);
    REQUIRE(t2.counts().copy_ctor == 0);
    REQUIRE(t2.counts().copy_assign == 1);
    REQUIRE(t2.counts().move_ctor == 0);
    REQUIRE(t2.counts().move_assign == 0);
  }

  TEST_CASE("Move construction") {
    tracer t1;

    tracer t2{std::move(t1)};

    REQUIRE(t1.counts().ctor == 1);
    REQUIRE(t1.counts().dtor == 0);
    REQUIRE(t1.counts().copy_ctor == 0);
    REQUIRE(t1.counts().copy_assign == 0);
    REQUIRE(t1.counts().move_ctor == 0);
    REQUIRE(t1.counts().move_assign == 0);

    REQUIRE(t2.counts().ctor == 1);
    REQUIRE(t2.counts().dtor == 0);
    REQUIRE(t2.counts().copy_ctor == 0);
    REQUIRE(t2.counts().copy_assign == 0);
    REQUIRE(t2.counts().move_ctor == 1);
    REQUIRE(t2.counts().move_assign == 0);
  }

  TEST_CASE("Move assignment") {
    tracer t1, t2;

    t2 = std::move(t1);

    REQUIRE(t1.counts().ctor == 1);
    REQUIRE(t1.counts().dtor == 0);
    REQUIRE(t1.counts().copy_ctor == 0);
    REQUIRE(t1.counts().copy_assign == 0);
    REQUIRE(t1.counts().move_ctor == 0);
    REQUIRE(t1.counts().move_assign == 0);

    REQUIRE(t2.counts().ctor == 1);
    REQUIRE(t2.counts().dtor == 0);
    REQUIRE(t2.counts().copy_ctor == 0);
    REQUIRE(t2.counts().copy_assign == 0);
    REQUIRE(t2.counts().move_ctor == 0);
    REQUIRE(t2.counts().move_assign == 1);
  }

} // TEST_SUITE
