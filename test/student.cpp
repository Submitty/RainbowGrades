#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "gradeable.h"
#include "student.h"

using Catch::Matchers::WithinRel;
constexpr double rtol = 1E-6;

TEST_CASE("nonzero counts") {
  GradeableList g(10, 1);
  g.setCorrespondence("1");
  g.setCorrespondence("2");
  g.setCorrespondence("3");
  g.setMaximum("1", 1);
  g.setMaximum("2", 1);
  // setting maximum score to <=0 means that the item is extra credit
  g.setMaximum("3", 0);
  g.setScaleMaximum("3", 1);
  float nonzero_sum =0;
  int nonzero_count = 0;
  int non_extra_credit_count = 0;
  getNonzeroCounts(g, nonzero_sum, nonzero_count, non_extra_credit_count);
  // nonzero sum takes the max(scaleMaximum, itemMaximum)
  REQUIRE_THAT(nonzero_sum, WithinRel(3,rtol));
  REQUIRE(nonzero_count == 3);
  // non_extra_credit_items takes only items with maximum <=0
  REQUIRE(non_extra_credit_count == 2);
}

TEST_CASE("calculate score percentages") {
  GradeableList g(10, 1);
  g.setCorrespondence("1");
  g.setCorrespondence("2");
  g.setCorrespondence("3");
  g.setMaximum("1", 1);
  g.setMaximum("2", 1);
  //g.setMaximum("3", 0);
  g.setScaleMaximum("3", 1);


}