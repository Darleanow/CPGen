#include <catch2/catch_test_macros.hpp>

TEST_CASE("Basic assertion", "[sample]") {
  REQUIRE(1 + 1 == 2);
}

TEST_CASE("String check", "[sample]") {
  std::string hello = "Hello";
  REQUIRE(hello.size() == 5);
}
