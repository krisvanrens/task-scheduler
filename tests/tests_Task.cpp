#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "../source/Task.hpp"
#include "Tracer.hpp"

TEST_CASE("Construction") {
  Task x{Tracer{}};

  REQUIRE(true);
}
