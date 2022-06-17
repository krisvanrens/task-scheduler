#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/MultiQueue.hpp"

#include <doctest/doctest.h>

#include <stdexcept>
#include <utility>

using Queue = MultiQueue<int, 100>;

TEST_SUITE("MultiQueue") {
  TEST_CASE("Construction") {
    Queue x1{1};
    Queue x2{10};
  }

  TEST_CASE("Construction (failure cases)") {
    CHECK_THROWS_AS(Queue{0}, std::underflow_error);
    CHECK_THROWS_AS(Queue{10'000}, std::overflow_error);
  }

  TEST_CASE("Move construction") {
    Queue x1{4};
    Queue x2{std::move(x1)};
  }

  TEST_CASE("Move assignment") {
    Queue x1{4}, x2{2};

    x2 = std::move(x1);
  }

} // TEST_SUITE
