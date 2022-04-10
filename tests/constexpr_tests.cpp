#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#ifdef TEST_NONSTATIC_REQUIRE
#define STATIC_REQUIRE(assertion) REQUIRE(assertion)
#else
#define STATIC_REQUIRE(assertion) static_assert(assertion, #assertion)
#endif

TEST_CASE("XXX") {
  STATIC_REQUIRE(true);
}
