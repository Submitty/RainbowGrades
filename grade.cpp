#include "grade.h"
#include "exceptions.h"

Grade::Grade(const std::string &v) : value(std::move(v)) {
  if (value != "A" && value != "A-" && value != "B+" && value != "B" &&
      value != "B-" && value != "C+" && value != "C" && value != "C-" &&
      value != "D+" && value != "D" && value != "F") {
    throw rg::InvalidGradeException("Invalid grade: " + value);
  }
}

// sorting function for letter grades
// higher grades are "less than" lower grades
bool operator<(const Grade &a, const Grade &b) {
  if (a.value == b.value)
    return false;

  if (a.value == "A")
    return true;
  if (b.value == "A")
    return false;
  if (a.value == "A-")
    return true;
  if (b.value == "A-")
    return false;

  if (a.value == "B+")
    return true;
  if (b.value == "B+")
    return false;
  if (a.value == "B")
    return true;
  if (b.value == "B")
    return false;
  if (a.value == "B-")
    return true;
  if (b.value == "B-")
    return false;

  if (a.value == "C+")
    return true;
  if (b.value == "C+")
    return false;
  if (a.value == "C")
    return true;
  if (b.value == "C")
    return false;
  if (a.value == "C-")
    return true;
  if (b.value == "C-")
    return false;

  if (a.value == "D+")
    return true;
  if (b.value == "D+")
    return false;
  if (a.value == "D")
    return true;
  if (b.value == "D")
    return false;

  if (a.value == "F")
    return true;
  if (b.value == "F")
    return false;

  return false;
}
