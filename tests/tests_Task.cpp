#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <utility>

#include "../source/Task.hpp"
#include "Tracer.hpp"

class Callable final : public Tracer {
  bool& is_called_;

public:
  explicit Callable(bool& is_called)
    : Tracer{}
    , is_called_{is_called} {
  }

  void operator()() {
    is_called_ = true;
  }
};

static bool free_function_is_called = false;

void free_function() {
  free_function_is_called = true;
}

TEST_CASE("Default construction") {
  Task<void()> t;

  CHECK_THROWS(t());
}

TEST_CASE("Construction from function object") {
  bool is_called = false;

  Task<void()> t{Callable{is_called}};

  REQUIRE(!is_called);

  t();

  CHECK(is_called);
}

TEST_CASE("Construction from lambda") {
  bool is_called = false;

  Task<void()> t{[&] { is_called = true; }};

  REQUIRE(!is_called);

  t();

  CHECK(is_called);
}

TEST_CASE("Construction from free function") {
  free_function_is_called = false;

  Task<void()> t{free_function};

  REQUIRE(!free_function_is_called);

  t();

  CHECK(free_function_is_called);
}

TEST_CASE("Move construction") {
  bool is_called = false;

  Task<void()> t1{Callable{is_called}};
  Task<void()> t2{std::move(t1)};

  REQUIRE(!is_called);

  CHECK_THROWS(t1());
  t2();

  CHECK(is_called);
}

TEST_CASE("Move assignment") {
  bool is_called = false;

  Task<void()> t1{Callable{is_called}}, t2;

  t2 = std::move(t1);

  REQUIRE(!is_called);

  CHECK_THROWS(t1());
  t2();

  CHECK(is_called);
}
