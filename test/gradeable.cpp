#include "gradeable.h"
#include "exceptions.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;
// constexpr double rtol = 1E-6;
constexpr double abs_tol = 1E-8;

TEST_CASE("GradeableList no correspondence") {
  // functions have inconsistent behavior when getting
  // a gradeable that doesn't currently exist.
  GradeableList g;
  REQUIRE(g.getCount() == 0);
  REQUIRE(g.getPercent() == 0.0f);
  REQUIRE_THAT(g.getBucketPercentageUpperClamp(), WithinAbs(0.0f, abs_tol));
  REQUIRE_THAT(g.getMaximum(), WithinAbs(0.0f, abs_tol));
  REQUIRE(g.getRemoveLowest() == 0);
  REQUIRE(g.getID(0) == "");
  REQUIRE(g.hasCorrespondence("test") == false);
  REQUIRE(g.getItemMaximum("test") == 0);
  REQUIRE(g.getScaleMaximum("test") == -1);
  REQUIRE(g.getItemPercentage("test") == -1);
  REQUIRE(g.hasSortedWeight() == false);
  g.setRemoveLowest(10);
  REQUIRE(g.getRemoveLowest() == 10);
  g.setBucketPercentageUpperClamp(0.5f);
  REQUIRE_THAT(g.getBucketPercentageUpperClamp(), WithinAbs(0.5f, abs_tol));
  g.addSortedWeight(0.5f);
  REQUIRE(g.hasSortedWeight() == true);
  REQUIRE_THAT(g.getSortedWeight(0), WithinAbs(0.5f, abs_tol));
  // apis that take ID are not tested as they need a correspondence to be set
}
TEST_CASE("GradeableList with correspondence") {
  GradeableList g(10, 1);
  g.setCorrespondence("test");
  g.setMaximum("test", 100);
  g.setScaleMaximum("test", 90);
  g.setItemPercentage("test", 80);
  g.setClamp("test", 70);
  g.setReleased("test", true);
  REQUIRE(g.hasCorrespondence("test") == true);
  REQUIRE_THAT(g.getItemMaximum("test"), WithinAbs(100.0f, abs_tol));
  REQUIRE_THAT(g.getScaleMaximum("test"), WithinAbs(90.0f, abs_tol));
  REQUIRE_THAT(g.getItemPercentage("test"), WithinAbs(80.0f, abs_tol));
  REQUIRE_THAT(g.getClamp("test"), WithinAbs(70.0f, abs_tol));
  REQUIRE(g.isReleased("test") == true);
  // current implementation is a failure to reset values of a given gradeable

  g.setCorrespondence("test2");
  g.setMaximum("test2", 78);
  g.setScaleMaximum("test2", 95);
  g.setItemPercentage("test2", 34);
  g.setClamp("test2", 17);
  g.setReleased("test2", false);

  // check that test unmodified after setting test2
  REQUIRE(g.hasCorrespondence("test") == true);
  REQUIRE_THAT(g.getItemMaximum("test"), WithinAbs(100.0f, abs_tol));
  REQUIRE_THAT(g.getScaleMaximum("test"), WithinAbs(90.0f, abs_tol));
  REQUIRE_THAT(g.getItemPercentage("test"), WithinAbs(80.0f, abs_tol));
  REQUIRE_THAT(g.getClamp("test"), WithinAbs(70.0f, abs_tol));
  REQUIRE(g.isReleased("test") == true);

 // check that values of test2 are correct
  REQUIRE(g.hasCorrespondence("test2") == true);
  REQUIRE_THAT(g.getItemMaximum("test2"), WithinAbs(78.0f, abs_tol));
  REQUIRE_THAT(g.getScaleMaximum("test2"), WithinAbs(95.0f, abs_tol));
  REQUIRE_THAT(g.getItemPercentage("test2"), WithinAbs(34.0f, abs_tol));
  REQUIRE_THAT(g.getClamp("test2"), WithinAbs(17.0f, abs_tol));
  REQUIRE(g.isReleased("test2") == false);


  // current implementation is a failure to reset values of a given gradeable
}