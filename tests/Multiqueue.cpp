#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/Multiqueue.hpp"

#include <doctest/doctest.h>

#include <stdexcept>
#include <utility>

using ValueType = unsigned int;
using Queue     = Multiqueue<ValueType, 10>;

TEST_SUITE("Multiqueue") {
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

  TEST_CASE("Getting the maximum queue size") {
    CHECK(Multiqueue<int, 1>{1}.max_queue_size() == 1);
    CHECK(Multiqueue<int, 2>{1}.max_queue_size() == 2);
    CHECK(Multiqueue<int, 10>{1}.max_queue_size() == 10);
  }

  TEST_CASE("Getting the maximum capacity") {
    CHECK(Multiqueue<int, 100>{1}.max_capacity() == 100);
    CHECK(Multiqueue<int, 100>{2}.max_capacity() == 200);
    CHECK(Multiqueue<int, 100>{10}.max_capacity() == 1'000);
    CHECK(Multiqueue<int, 1024>{1}.max_capacity() == 1'024);
    CHECK(Multiqueue<int, 1024>{2}.max_capacity() == 2'048);
    CHECK(Multiqueue<int, 1024>{10}.max_capacity() == 10'240);
    CHECK(Multiqueue<int, 8192>{1024}.max_capacity() == 8'388'608);
  }

  TEST_CASE("Pushing elements (single queue)") {
    Queue x{1};

    REQUIRE(x.max_capacity() == x.max_queue_size());

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      CHECK(x.push(42));
    }

    CHECK_FALSE(x.push(42));
    CHECK_FALSE(x.push(42));
  }

  TEST_CASE("Pushing elements (multiple queues)") {
    Queue x{10};

    REQUIRE(x.max_capacity() == 10 * x.max_queue_size());

    for (unsigned int i = 0; i < (10 * x.max_queue_size()); i++) {
      CHECK(x.push(42));
    }

    CHECK_FALSE(x.push(42));
    CHECK_FALSE(x.push(42));
  }

  TEST_CASE("Popping elements queue-indexed (single queue)") {
    Queue x{1};

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      REQUIRE(x.push(i));
    }

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      const auto element = x.pop(0);
      REQUIRE(element.has_value());
      CHECK(element.value() == i);
    }

    CHECK_FALSE(x.pop(0).has_value());
  }

  TEST_CASE("Popping elements queue-indexed (multiple queues)") {
    constexpr unsigned int NUM_QUEUES = 10;
    Queue                  x{NUM_QUEUES};

    for (unsigned int i = 0; i < (NUM_QUEUES * x.max_queue_size()); i++) {
      REQUIRE(x.push(i));
    }

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      for (unsigned int queue_idx = 0; queue_idx < NUM_QUEUES; queue_idx++) {
        const auto element = x.pop(queue_idx);
        REQUIRE(element.has_value());
        CHECK(element.value() == ((i * NUM_QUEUES) + queue_idx));
      }
    }

    for (unsigned int queue_idx = 0; queue_idx < NUM_QUEUES; queue_idx++) {
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

  TEST_CASE("Popping elements with work stealing") {
    Multiqueue<unsigned int, 5> x{2};

    for (unsigned int i = 0; i < (2 * x.max_queue_size()); i++) {
      REQUIRE(x.push(i));
    }

    const auto pop_validate = [&](unsigned int queue_idx, unsigned int expected) {
      const auto element = x.pop(queue_idx);
      REQUIRE(element.has_value());
      CHECK(element.value() == expected);
    };

    // First deplete about half of the first queue.
    pop_validate(0, 0);
    pop_validate(0, 2);
    pop_validate(0, 4);

    // Deplete the second queue.
    pop_validate(1, 1);
    pop_validate(1, 3);
    pop_validate(1, 5);
    pop_validate(1, 7);
    pop_validate(1, 9);

    // The second queue is now empty, elements should be taken from the first queue.
    pop_validate(1, 6);
    pop_validate(1, 8);

    // The multi-queue is empty: subsequent pops should return no result.
    CHECK_FALSE(x.pop(0).has_value());
    CHECK_FALSE(x.pop(0).has_value());
    CHECK_FALSE(x.pop(1).has_value());
    CHECK_FALSE(x.pop(1).has_value());
  }

} // TEST_SUITE
