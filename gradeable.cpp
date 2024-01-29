#include <string>
#include <iostream>
#include <numeric>
#include <sstream>
#include "gradeable.h"
#include "exceptions.h"

std::string gradeable_enum_to_string(const GRADEABLE_ENUM &g) {
  switch (g) {
  case GRADEABLE_ENUM::HOMEWORK: return "HOMEWORK";
  case GRADEABLE_ENUM::ASSIGNMENT: return "ASSIGNMENT";
  case GRADEABLE_ENUM::PROBLEM_SET: return "PROBLEM_SET";
  case GRADEABLE_ENUM::QUIZ: return "QUIZ";
  case GRADEABLE_ENUM::TEST: return "TEST";
  case GRADEABLE_ENUM::EXAM: return "EXAM";
  case GRADEABLE_ENUM::EXERCISE: return "EXERCISE";
  case GRADEABLE_ENUM::LECTURE_EXERCISE: return "LECTURE_EXERCISE";
  case GRADEABLE_ENUM::READING: return "READING";
  case GRADEABLE_ENUM::WORKSHEET: return "WORKSHEET";
  case GRADEABLE_ENUM::LAB: return "LAB";
  case GRADEABLE_ENUM::RECITATION: return "RECITATION";
  case GRADEABLE_ENUM::PROJECT: return "PROJECT";
  case GRADEABLE_ENUM::PARTICIPATION: return "PARTICIPATION";
  case GRADEABLE_ENUM::NOTE: return "NOTE";
  case GRADEABLE_ENUM::NONE: return "NONE";
    // note we don't have a default case so that the compiler will warn if missing a case
  }
  std::cerr << "ERROR!  UNKNOWN GRADEABLE" << std::endl;
  exit(0);
}

bool string_to_gradeable_enum(const std::string &s, GRADEABLE_ENUM &return_value) {
  // TODO write testcase and replace ad-hoc code with tolower from string_utility.h
  std::string s2;
  for (unsigned int i = 0; i < s.size(); ++i) {
    s2.push_back(std::tolower(s[i]));
  }
  std::replace( s2.begin(), s2.end(), '-', '_');
  if (s2 == "hw" || s2 == "homework")          { return_value = GRADEABLE_ENUM::HOMEWORK;          return true;  }
  if (s2 == "assignment")                      { return_value = GRADEABLE_ENUM::ASSIGNMENT;        return true;  }
  if (s2 == "problem_set")                     { return_value = GRADEABLE_ENUM::PROBLEM_SET;       return true;  }
  if (s2 == "quiz" || s2 == "quizze")          { return_value = GRADEABLE_ENUM::QUIZ;              return true;  }
  if (s2 == "test")                            { return_value = GRADEABLE_ENUM::TEST;              return true;  }
  if (s2 == "exam")                            { return_value = GRADEABLE_ENUM::EXAM;              return true;  }
  if (s2 == "exercise")                        { return_value = GRADEABLE_ENUM::EXERCISE;          return true;  }
  if (s2 == "lecture_exercise")                { return_value = GRADEABLE_ENUM::LECTURE_EXERCISE;  return true;  }
  if (s2 == "reading")                         { return_value = GRADEABLE_ENUM::READING;           return true;  }
  if (s2 == "worksheet")                       { return_value = GRADEABLE_ENUM::WORKSHEET;         return true;  }
  if (s2 == "lab")                             { return_value = GRADEABLE_ENUM::LAB;               return true;  }
  if (s2 == "recitation")                      { return_value = GRADEABLE_ENUM::RECITATION;        return true;  }
  if (s2 == "project")                         { return_value = GRADEABLE_ENUM::PROJECT;           return true;  }
  if (s2 == "participation")                   { return_value = GRADEABLE_ENUM::PARTICIPATION;     return true;  }
  if (s2 == "instructor_note" || s2 == "note") { return_value = GRADEABLE_ENUM::NOTE;              return true;  }
  if (s2 == "note")                            { return_value = GRADEABLE_ENUM::NOTE;              return true;  }
  if (s2.substr(0,4) == "none")                { return_value = GRADEABLE_ENUM::NOTE;              return true;  }
  return false;
}

Gradeable& GradeableList::getGradeable(const GradeableID& id) {
  // See Effective C++ this is safe since we know that GradeableList is not const
  return const_cast<Gradeable&>(static_cast<const GradeableList&>(*this).getGradeable(id));
}
const Gradeable& GradeableList::getGradeable(const GradeableID& id) const {
  auto itr = gradeables_.find(id);
  if(itr == gradeables_.end()){
    std::stringstream ss;
    ss << "GradeableList::getGradeable: missing gradeable for ID: " << id.value();
    throw rg::InvalidID(ss.str());
  }
  return itr->second;
}

size_t GradeableList::getCount() const {
  return count;
}
float GradeableList::getBucketPercentageUpperClamp() const { return this->bucket_percentage_upper_clamp; }
float GradeableList::getPercent() const { return percent; }
float GradeableList::getExpectedTotalPoints() const {
  if(gradeables_.size() == 0) { return 0; }
  auto max_sum = std::accumulate(gradeables_.begin(), gradeables_.end(), 0.0f,
                  [](float sum, const auto& g) ->float { return sum + g.second.maximum; });
  // gives the average maximum score per gradeable * the total number of gradeables
  return max_sum * (float)getCount() / (float)gradeables_.size();
}
int GradeableList::getRemoveLowest() const { return remove_lowest; }
GradeableID GradeableList::getID(GradeableIndex index) const {
  auto it = std::find_if(gradeables_.begin(), gradeables_.end(), [index](const auto &g) {
    return g.second.correspondence.index == index;
  });
  if(it != gradeables_.end()) { return it->first; }
  return GradeableID{""};
}
bool GradeableList::hasCorrespondence(const GradeableID &id) const {
  if(gradeables_.size() == 0 ) { return false; }
  auto itr = gradeables_.find(id);
  if (itr == gradeables_.end()) {
    return false;
  }
  return true;
}
const Correspondence &
GradeableList::getCorrespondence(const GradeableID &id) const {
  auto& gradeable = getGradeable(id);
  return gradeable.correspondence;
}
bool GradeableList::isReleased(const GradeableID &id) const {
  if(gradeables_.size() == 0) { return false; }
  return getGradeable(id).released;
}
float GradeableList::getItemMaximum(const GradeableID &id) const {
  if(gradeables_.size() == 0) { return 0; }
  return getGradeable(id).maximum;
}
float GradeableList::getScaleMaximum(const GradeableID &id) const {
  if(gradeables_.size() == 0) { return -1;}
  return getGradeable(id).scale_maximum;
}
float GradeableList::getItemPercentage(const GradeableID &id) const {
  if(gradeables_.size() == 0) { return -1; }
  return getGradeable(id).item_percentage;
}
float GradeableList::getClamp(const GradeableID &id) const {
  if(gradeables_.size() == 0) { return 0; }
  return getGradeable(id).clamp;
}
float GradeableList::getSortedWeight(unsigned int position) {
  // FIXME this should be enforced by the in gradeableList interface
  assert(position < sorted_weights.size());
  return sorted_weights[position];
}
bool GradeableList::hasSortedWeight() {
  return !sorted_weights.empty();
}
void GradeableList::setRemoveLowest(int r) { remove_lowest=r; }

GradeableIndex GradeableList::setCorrespondence(const GradeableID &id) {
  const auto& [itr, inserted] = gradeables_.try_emplace(id, Gradeable{});
  if(!inserted){
    std::stringstream ss;
    ss << "GradeableList::setCorrespondence: gradeable with ID " << id.value() <<" already exists.";
    throw rg::InvalidID(ss.str());
  }
  auto& gradeable = itr->second;
  auto index = GradeableIndex{gradeables_.size()-1};
  gradeable.correspondence = Correspondence{index, ""};
  return index;
}
void GradeableList::setBucketPercentageUpperClamp(
    float bucket_percentage_upper_clamp) {
  this->bucket_percentage_upper_clamp = bucket_percentage_upper_clamp;
}
void GradeableList::setCorrespondenceName(const GradeableID &id,
                                      const std::string &name) {
  auto& gradeable = getGradeable(id);
  gradeable.correspondence.name = name;
}
void GradeableList::setReleased(const GradeableID &id, bool is_released) {
  getGradeable(id).released = is_released;
}
void GradeableList::setMaximum(const GradeableID &id, float maximum) {
  getGradeable(id).maximum = maximum;
}
void GradeableList::setScaleMaximum(const GradeableID &id, float scale_maximum) {
  getGradeable(id).scale_maximum = scale_maximum;
}
void GradeableList::setItemPercentage(const GradeableID &id,
                                  float item_percentage) {
  getGradeable(id).item_percentage = item_percentage;
}
void GradeableList::setClamp(const GradeableID &id, float clamp) {
  getGradeable(id).clamp = clamp;
}
void GradeableList::addSortedWeight(float weight) {
  sorted_weights.push_back(weight);
}

size_t GradeableList::getExtraCreditCount() const {
  return std::accumulate(gradeables_.begin(), gradeables_.end(), (size_t)0,
                         [](size_t sum, const auto &g) -> size_t {
                           return sum + (g.second.isExtraCredit());
                         });
}
size_t GradeableList::getNormalCount() const {
  // note: we need to use the size of the gradeables, not the count
  // which may be higher
  return gradeables_.size() - getExtraCreditCount();
}
float GradeableList::getExtraCreditPoints() const {
  return std::accumulate(gradeables_.begin(), gradeables_.end(), 0.0f,
                         [](float sum, const auto &g) -> float {
                           return sum + (g.second.isExtraCredit()
                                             ? g.second.getScaledMaximum()
                                             : 0.0f);
                         });
}

float GradeableList::getActivePoints() const {
  return std::accumulate(gradeables_.begin(), gradeables_.end(), 0.0f,
                         [](float sum, const auto &g) -> float {
                           return sum + (g.second.isActive()
                                         ? g.second.getScaledMaximum()
                                         : 0.0f);
                         });
}
size_t GradeableList::getActiveCount() const {
  return std::accumulate(gradeables_.begin(), gradeables_.end(), (size_t)0,
                         [](size_t sum, const auto &g) -> size_t {
                           return sum + (g.second.isActive());
                         });
}


float GradeableList::getTotalPoints() const {
  return std::accumulate(gradeables_.begin(), gradeables_.end(), 0.0f,
                         [](float sum, const auto &g) -> float {
                           return sum + g.second.getScaledMaximum();
                         });
}
float GradeableList::getNormalPoints() const {
  return getTotalPoints() - getExtraCreditPoints();
}

void LookupGradeable(const GradeableID &id,
                     GRADEABLE_ENUM &g_e, GradeableIndex &i) {
  for (std::size_t k = 0; k < ALL_GRADEABLES.size(); k++) {
    GRADEABLE_ENUM e = ALL_GRADEABLES[k];
    GradeableList g = GRADEABLES[e];
    if (g.hasCorrespondence(id)) {
      g_e = e;
      i = g.getCorrespondence(id).index;
      return;
    }
  }
  assert (0);
}