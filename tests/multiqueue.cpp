#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/multiqueue.hpp"

#include <doctest/doctest.h>

#include <stdexcept>
#include <utility>

using namespace ts;

using test_queue = multiqueue<unsigned int, 10>;

TEST_SUITE("multiqueue") {
  TEST_CASE("Construction") {
    test_queue x1{1};
    test_queue x2{10};
  }

  TEST_CASE("Construction (failure cases)") {
    CHECK_THROWS_AS(test_queue{0}, std::underflow_error);
    CHECK_THROWS_AS(test_queue{10'000}, std::overflow_error);
  }

  TEST_CASE("Move construction") {
    test_queue x{4};

    REQUIRE(x.num_queues() == 4);

    test_queue y{std::move(x)};

    CHECK(y.num_queues() == 4);
  }

  TEST_CASE("Move assignment") {
    test_queue x{4}, y{2};

    REQUIRE(x.num_queues() == 4);
    REQUIRE(y.num_queues() == 2);

    y = std::move(x);

    CHECK(y.num_queues() == 4);
  }

  TEST_CASE("Getting the maximum queue size") {
    CHECK(multiqueue<int, 1>{1}.max_queue_size() == 1);
    CHECK(multiqueue<int, 2>{1}.max_queue_size() == 2);
    CHECK(multiqueue<int, 10>{1}.max_queue_size() == 10);
  }

  TEST_CASE("Getting the number of queues") {
    CHECK(multiqueue<int, 1>{1}.num_queues() == 1);
    CHECK(multiqueue<int, 1>{2}.num_queues() == 2);
    CHECK(multiqueue<int, 1>{10}.num_queues() == 10);
  }

  TEST_CASE("Getting the maximum capacity") {
    CHECK(multiqueue<int, 100>{1}.max_capacity() == 100);
    CHECK(multiqueue<int, 100>{2}.max_capacity() == 200);
    CHECK(multiqueue<int, 100>{10}.max_capacity() == 1'000);
    CHECK(multiqueue<int, 1024>{1}.max_capacity() == 1'024);
    CHECK(multiqueue<int, 1024>{2}.max_capacity() == 2'048);
    CHECK(multiqueue<int, 1024>{10}.max_capacity() == 10'240);
    CHECK(multiqueue<int, 8192>{1024}.max_capacity() == 8'388'608);
  }

  TEST_CASE("Getting the empty state") {
    test_queue x{1};

    CHECK(x.empty());

    CHECK(x.push(42u));

    CHECK_FALSE(x.empty());
  }

  TEST_CASE("Getting the size (single queue)") {
    test_queue x{1};

    REQUIRE(x.max_capacity() == x.max_queue_size());

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      CHECK(x.size() == i);
      REQUIRE(x.push(42u));
      CHECK(x.size() == (i + 1));
    }
  }

  TEST_CASE("Getting the size (multiple queues)") {
    test_queue x{10};

    REQUIRE(x.max_capacity() == 10 * x.max_queue_size());

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      CHECK(x.size() == i);
      REQUIRE(x.push(42u));
      CHECK(x.size() == (i + 1));
    }
  }

  TEST_CASE("Pushing elements (single queue)") {
    test_queue x{1};

    REQUIRE(x.max_capacity() == x.max_queue_size());

    for (unsigned int i = 0; i < x.max_queue_size(); i++) {
      CHECK(x.push(42u));
    }

    CHECK_FALSE(x.push(42u));
    CHECK_FALSE(x.push(42u));
  }

  TEST_CASE("Pushing elements (multiple queues)") {
    test_queue x{10};

    REQUIRE(x.max_capacity() == 10 * x.max_queue_size());

    for (unsigned int i = 0; i < (10 * x.max_queue_size()); i++) {
      CHECK(x.push(42u));
    }

    CHECK_FALSE(x.push(42u));
    CHECK_FALSE(x.push(42u));
  }

  TEST_CASE("Popping elements queue-indexed (single queue)") {
    test_queue x{1};

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
    test_queue             x{NUM_QUEUES};

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

    CHECK_THROWS_AS(pop(test_queue{1}, 1), std::out_of_range);
    CHECK_THROWS_AS(pop(test_queue{2}, 2), std::out_of_range);
    CHECK_THROWS_AS(pop(test_queue{10}, 10), std::out_of_range);
    CHECK_THROWS_AS(pop(test_queue{10}, 100), std::out_of_range);
  }

  TEST_CASE("Popping elements with work stealing") {
    multiqueue<unsigned int, 5> x{2};

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

  TEST_CASE("Flushing the queue") {
    multiqueue<unsigned int, 5> x{2};

    REQUIRE(x.empty());

    for (unsigned int i = 0; i < (2 * x.max_queue_size()); i++) {
      REQUIRE(x.push(i));
    }

    REQUIRE_FALSE(x.empty());
    REQUIRE(x.size() == (2 * x.max_queue_size()));

    x.flush();

    CHECK(x.empty());
    CHECK(x.size() == 0);
  }

} // TEST_SUITE
