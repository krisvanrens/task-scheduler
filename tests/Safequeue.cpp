#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/Safequeue.hpp"

#include <doctest/doctest.h>

#include <utility>

struct MoveOnly {
  MoveOnly() = default;

  MoveOnly(MoveOnly&&)            = default;
  MoveOnly& operator=(MoveOnly&&) = default;
};

using Queue = Safequeue<unsigned int, 10>;

TEST_SUITE("Safequeue") {
  TEST_CASE("Default construction") {
    Queue x;

    REQUIRE(x.max_size() == 10);
  }

  TEST_CASE("Get maximum length") {
    CHECK(Safequeue<int, 1>{}.max_size() == 1);
    CHECK(Safequeue<int, 2>{}.max_size() == 2);
    CHECK(Safequeue<int, 10>{}.max_size() == 10);
    CHECK(Safequeue<int, 100>{}.max_size() == 100);
    CHECK(Safequeue<int, 8192>{}.max_size() == 8192);
  }

  TEST_CASE("Get size") {
    Queue x;

    CHECK(x.size() == 0);

    REQUIRE(x.push(42u));
    CHECK(x.size() == 1);

    REQUIRE(x.push(42u));
    CHECK(x.size() == 2);
  }

  TEST_CASE("Get empty state") {
    Queue x;

    CHECK(x.empty());

    REQUIRE(x.push(42u));
    CHECK_FALSE(x.empty());

    REQUIRE(x.pop().has_value());
    CHECK(x.empty());
  }

  TEST_CASE("Pushing elements") {
    Queue x;

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
    Queue x;

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
    Safequeue<MoveOnly, 3> x;

    CHECK(x.push(MoveOnly{}));
    CHECK(x.push(MoveOnly{}));
    CHECK(x.push(MoveOnly{}));

    const auto pop = [&] {
      auto element = x.pop();
      REQUIRE(element.has_value());
      [[maybe_unused]] const MoveOnly m = std::move(element.value());
    };

    pop();
    pop();
    pop();
  }

} // TEST_SUITE
