#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <utility>

#include "Tracer.hpp"

TEST_CASE("Construction") {
  Tracer t;

  REQUIRE(t.counts().ctor == 1);
  REQUIRE(t.counts().dtor == 0);
  REQUIRE(t.counts().copy_ctor == 0);
  REQUIRE(t.counts().copy_assign == 0);
  REQUIRE(t.counts().move_ctor == 0);
  REQUIRE(t.counts().move_assign == 0);
}

TEST_CASE("Copy construction") {
  Tracer t1;

  Tracer t2{t1};

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
  Tracer t1, t2;

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
  Tracer t1;

  Tracer t2{std::move(t1)};

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
  Tracer t1, t2;

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
