#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "../source/Task.hpp"
#include "Tracer.hpp"

#include <utility>

TEST_CASE("Default construction") {
  Task t;

  REQUIRE(true);
}

TEST_CASE("Construction") {
  Task t{Tracer{}};

  REQUIRE(true);
}

TEST_CASE("Move construction") {
  Task t1{Tracer{}};
  Task t2{std::move(t1)};

  REQUIRE(true);
}

TEST_CASE("Move assignment") {
  Task t1{Tracer{}}, t2;

  t2 = std::move(t1);

  REQUIRE(true);
}
