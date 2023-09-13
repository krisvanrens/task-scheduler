#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../source/completion_token.hpp"

#include <doctest/doctest.h>

#include <chrono>
#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>

using namespace ts;

TEST_SUITE("completion_data") {
  using namespace detail;

  TEST_CASE("Construction") {
    completion_data d;

    CHECK_FALSE(d.is_completed());
  }

  TEST_CASE("Triggering") {
    completion_data d;

    CHECK_FALSE(d.is_completed());

    d.trigger_completion();

    CHECK(d.is_completed());
  }

  TEST_CASE("Exception storage") {
    completion_data d;

    CHECK_FALSE(d.exception());

    try {
      throw std::invalid_argument{"test"};
    } catch (...) {
      d.exception() = std::current_exception();
    }

    CHECK(d.exception());
  }
}

TEST_SUITE("completion_token") {
  TEST_CASE("Construction") {
    completion_token t{std::make_shared<detail::completion_data>()};

    CHECK_FALSE(t);
  }

  TEST_CASE("Completion handling") {
    using namespace std::chrono;

    auto             data = std::make_shared<detail::completion_data>();
    completion_token t{data};

    CHECK_FALSE(t);

    std::jthread th{[&data] {
      std::this_thread::sleep_for(milliseconds(100));
      data->trigger_completion();
    }};

    const auto time_start = system_clock::now();

    t.wait();

    const auto time_end = system_clock::now();

    CHECK(t);
    CHECK(duration_cast<milliseconds>(time_end - time_start).count() >= 100);
  }

  TEST_CASE("Exception handling") {
    auto             data = std::make_shared<detail::completion_data>();
    completion_token t{data};

    CHECK_FALSE(t.exception());

    try {
      throw std::invalid_argument{"test"};
    } catch (...) {
      data->exception() = std::current_exception();
    }

    REQUIRE(t.exception());

    try {
      std::rethrow_exception(*t.exception());
    } catch (const std::exception& error) {
      CHECK(std::string{error.what()} == "test");
    }
  }

} // TEST_SUITE
