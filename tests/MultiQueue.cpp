#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/MultiQueue.hpp"

#include <doctest/doctest.h>

#include <stdexcept>
#include <utility>

using ValueType = unsigned int;
using Queue     = MultiQueue<ValueType, 10>;

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

  TEST_CASE("Getting the maximum queue lenght") {
    CHECK(MultiQueue<int, 1>{1}.max_queue_length() == 1);
    CHECK(MultiQueue<int, 2>{1}.max_queue_length() == 2);
    CHECK(MultiQueue<int, 10>{1}.max_queue_length() == 10);
  }

  TEST_CASE("Getting the maximum capacity") {
    CHECK(MultiQueue<int, 100>{1}.max_capacity() == 100);
    CHECK(MultiQueue<int, 100>{2}.max_capacity() == 200);
    CHECK(MultiQueue<int, 100>{10}.max_capacity() == 1'000);
    CHECK(MultiQueue<int, 1024>{1}.max_capacity() == 1'024);
    CHECK(MultiQueue<int, 1024>{2}.max_capacity() == 2'048);
    CHECK(MultiQueue<int, 1024>{10}.max_capacity() == 10'240);
    CHECK(MultiQueue<int, 8192>{1024}.max_capacity() == 8'388'608);
  }

  TEST_CASE("Pushing elements (single queue)") {
    Queue x{1};

    REQUIRE(x.max_capacity() == x.max_queue_length());

    for (unsigned int i = 0; i < x.max_queue_length(); i++) {
      CHECK(x.push(42));
    }

    CHECK_FALSE(x.push(42));
    CHECK_FALSE(x.push(42));
  }

  TEST_CASE("Pushing elements (multiple queues)") {
    Queue x{10};

    REQUIRE(x.max_capacity() == 10 * x.max_queue_length());

    for (unsigned int i = 0; i < (10 * x.max_queue_length()); i++) {
      CHECK(x.push(42));
    }

    CHECK_FALSE(x.push(42));
    CHECK_FALSE(x.push(42));
  }

  TEST_CASE("Popping elements queue-indexed (single queue)") {
    Queue x{1};

    for (unsigned int i = 0; i < x.max_queue_length(); i++) {
      REQUIRE(x.push(i));
    }

    for (unsigned int i = 0; i < x.max_queue_length(); i++) {
      const auto element = x.pop(0);
      REQUIRE(element.has_value());
      CHECK(element.value() == i);
    }

    CHECK_FALSE(x.pop(0).has_value());
  }

  TEST_CASE("Popping elements queue-indexed (multiple queues)") {
    Queue x{10};

    for (unsigned int i = 0; i < (10 * x.max_queue_length()); i++) {
      REQUIRE(x.push(i));
    }

    for (unsigned int i = 0; i < x.max_queue_length(); i++) {
      for (unsigned int queue_idx = 0; queue_idx < 10; queue_idx++) {
        const auto element = x.pop(queue_idx);
        REQUIRE(element.has_value());
        CHECK(element.value() == ((i * 10) + queue_idx));
      }
    }

    for (unsigned int queue_idx = 0; queue_idx < 10; queue_idx++) {
      CHECK_FALSE(x.pop(queue_idx).has_value());
    }
  }

  TEST_CASE("Popping queue-indexed (failure cases)") {
    const auto pop = [](auto&& queue, unsigned int index) { [[maybe_unused]] auto result = queue.pop(index); };

    CHECK_THROWS_AS(pop(Queue{1}, 1), std::out_of_range);
    CHECK_THROWS_AS(pop(Queue{2}, 2), std::out_of_range);
    CHECK_THROWS_AS(pop(Queue{10}, 10), std::out_of_range);
    CHECK_THROWS_AS(pop(Queue{10}, 100), std::out_of_range);
  }

} // TEST_SUITE
