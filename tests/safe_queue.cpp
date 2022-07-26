#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/safe_queue.hpp"

#include <doctest/doctest.h>

#include <utility>

struct move_only {
  move_only() = default;

  move_only(move_only&&)            = default;
  move_only& operator=(move_only&&) = default;
};

using namespace ts;

using test_queue = safe_queue<unsigned int, 10>;

TEST_SUITE("safe_queue") {
  TEST_CASE("Default construction") {
    test_queue x;

    REQUIRE(x.max_size() == 10);
  }

  TEST_CASE("Get maximum length") {
    CHECK(safe_queue<int, 1>{}.max_size() == 1);
    CHECK(safe_queue<int, 2>{}.max_size() == 2);
    CHECK(safe_queue<int, 10>{}.max_size() == 10);
    CHECK(safe_queue<int, 100>{}.max_size() == 100);
    CHECK(safe_queue<int, 8192>{}.max_size() == 8192);
  }

  TEST_CASE("Get size") {
    test_queue x;

    CHECK(x.size() == 0);

    REQUIRE(x.push(42u));
    CHECK(x.size() == 1);

    REQUIRE(x.push(42u));
    CHECK(x.size() == 2);
  }

  TEST_CASE("Get empty state") {
    test_queue x;

    CHECK(x.empty());

    REQUIRE(x.push(42u));
    CHECK_FALSE(x.empty());

    REQUIRE(x.pop().has_value());
    CHECK(x.empty());
  }

  TEST_CASE("Pushing elements") {
    test_queue x;

    REQUIRE(x.size() == 0);

    for (unsigned int i = 0; i < 10; i++) {
      REQUIRE(x.size() == i);
      CHECK(x.push(42u));
    }

    REQUIRE(x.size() == 10);

    CHECK_FALSE(x.push(42u));
    CHECK_FALSE(x.push(42u));

    REQUIRE(x.size() == 10);
  }

  TEST_CASE("Popping elements") {
    test_queue x;

    for (unsigned int i = 0; i < 10; i++) {
      REQUIRE(x.size() == i);
      REQUIRE(x.push(i));
    }

    for (unsigned int i = 0; i < 10; i++) {
      const auto result = x.pop();
      CHECK(result.has_value());
      CHECK(result.value() == i);
    }

    CHECK_FALSE(x.pop().has_value());
    CHECK_FALSE(x.pop().has_value());
  }

  TEST_CASE("Move-only type handling") {
    safe_queue<move_only, 3> x;

    CHECK(x.push(move_only{}));
    CHECK(x.push(move_only{}));
    CHECK(x.push(move_only{}));

    const auto pop = [&] {
      auto element = x.pop();
      REQUIRE(element.has_value());
      [[maybe_unused]] const move_only m = std::move(element.value());
    };

    pop();
    pop();
    pop();
  }

  TEST_CASE("Flushing the queue") {
    test_queue x;

    for (unsigned int i = 0; i < 10; i++) {
      REQUIRE(x.size() == i);
      REQUIRE(x.push(i));
    }

    REQUIRE_FALSE(x.empty());
    REQUIRE(x.size() == 10);

    x.flush();

    CHECK(x.empty());
  }

} // TEST_SUITE
