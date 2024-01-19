#include <catch2/catch_test_macros.hpp>
#include <grade.h>

TEST_CASE("grade comparison") {
  Grade a("A");
  Grade a_minus("A-");
  Grade b_plus("B+");
  Grade b("B");
  Grade b_minus("B-");
  Grade c_plus("C+");
  Grade c("C");
  Grade c_minus("C-");
  Grade d_plus("D+");
  Grade d("D");
  Grade f("F");





  REQUIRE(a < b);
  REQUIRE(a < a_minus);
  REQUIRE(a_minus < b);
  REQUIRE(b_plus < b);
  REQUIRE(b_plus < b_minus);
  REQUIRE(b < c);
  REQUIRE(c < d);
  REQUIRE(d < f);

}

TEST_CASE("fails on invalid grade") {
  REQUIRE_THROWS(Grade("x"));
  // lowercase grades
  REQUIRE_THROWS(Grade("a"));
  REQUIRE_THROWS(Grade("a-"));
}