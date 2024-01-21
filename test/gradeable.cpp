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
  GradeableID testID("test");
  REQUIRE(g.getCount() == 0);
  REQUIRE(g.getPercent() == 0.0f);
  REQUIRE_THAT(g.getBucketPercentageUpperClamp(), WithinAbs(0.0f, abs_tol));
  REQUIRE_THAT(g.getExpectedTotalPoints(), WithinAbs(0.0f, abs_tol));
  REQUIRE(g.getRemoveLowest() == 0);
  REQUIRE(g.getID(GradeableIndex{0}) == GradeableID{""});
  REQUIRE(g.hasCorrespondence(testID) == false);
  REQUIRE(g.getItemMaximum(testID) == 0);
  REQUIRE(g.getScaleMaximum(testID) == -1);
  REQUIRE(g.getItemPercentage(testID) == -1);
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
  GradeableID testID("test");
  GradeableID testID2("test2");

  g.setCorrespondence(testID);
  g.setMaximum(testID, 100);
  g.setScaleMaximum(testID, 90);
  g.setItemPercentage(testID, 80);
  g.setClamp(testID, 70);
  g.setReleased(testID, true);
  REQUIRE(g.hasCorrespondence(testID) == true);
  REQUIRE_THAT(g.getItemMaximum(testID), WithinAbs(100.0f, abs_tol));
  REQUIRE_THAT(g.getScaleMaximum(testID), WithinAbs(90.0f, abs_tol));
  REQUIRE_THAT(g.getItemPercentage(testID), WithinAbs(80.0f, abs_tol));
  REQUIRE_THAT(g.getClamp(testID), WithinAbs(70.0f, abs_tol));
  REQUIRE(g.isReleased(testID) == true);
  // current implementation is a failure to reset values of a given gradeable

  g.setCorrespondence(testID2);
  g.setMaximum(testID2, 78);
  g.setScaleMaximum(testID2, 95);
  g.setItemPercentage(testID2, 34);
  g.setClamp(testID2, 17);
  g.setReleased(testID2, false);

  // check that test unmodified after setting test2
  REQUIRE(g.hasCorrespondence(testID) == true);
  REQUIRE_THAT(g.getItemMaximum(testID), WithinAbs(100.0f, abs_tol));
  REQUIRE_THAT(g.getScaleMaximum(testID), WithinAbs(90.0f, abs_tol));
  REQUIRE_THAT(g.getItemPercentage(testID), WithinAbs(80.0f, abs_tol));
  REQUIRE_THAT(g.getClamp(testID), WithinAbs(70.0f, abs_tol));
  REQUIRE(g.isReleased(testID) == true);

 // check that values of test2 are correct
  REQUIRE(g.hasCorrespondence(testID2) == true);
  REQUIRE_THAT(g.getItemMaximum(testID2), WithinAbs(78.0f, abs_tol));
  REQUIRE_THAT(g.getScaleMaximum(testID2), WithinAbs(95.0f, abs_tol));
  REQUIRE_THAT(g.getItemPercentage(testID2), WithinAbs(34.0f, abs_tol));
  REQUIRE_THAT(g.getClamp(testID2), WithinAbs(17.0f, abs_tol));
  REQUIRE(g.isReleased(testID2) == false);

  // current implementation is a failure to reset values of a given gradeable
}