#include <string>
#include <gradeable.h>
#include <iostream>

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

int Gradeable::getCount() const {
  return count;
}
float Gradeable::getBucketPercentageUpperClamp() const { return this->bucket_percentage_upper_clamp; }
float Gradeable::getPercent() const { return percent; }
float Gradeable::getMaximum() const {
  if (maximums.size() == 0) return 0;
  assert (maximums.size() > 0);
  float max_sum = 0;
  for (std::map<std::string,float>::const_iterator itr = maximums.begin();
       itr != maximums.end(); itr++) {
    max_sum += itr->second;
  }
  return max_sum * getCount() / maximums.size();
}
int Gradeable::getRemoveLowest() const { return remove_lowest; }
std::string Gradeable::getID(int index) const {
  std::map<std::string,std::pair<int,std::string> >::const_iterator itr = correspondences.begin();
  while (itr != correspondences.end()) {
    if (itr->second.first == index) return itr->first;
    itr++;
  }
  return "";
}
void Gradeable::isResubmit(int index, std::string &original_id,
                           std::string &resubmit_id,
                           float &autograde_replacement_percentage) {
  std::string id = getID(index);
  if (original_ids.find(id) == original_ids.end()) return;
  assert (resubmit_ids.find(id) != resubmit_ids.end());
  assert (autograde_replacement_percentages.find(id) != autograde_replacement_percentages.end());
  original_id = original_ids.find(id)->second;
  resubmit_id = resubmit_ids.find(id)->second;
  autograde_replacement_percentage = autograde_replacement_percentages.find(id)->second;
}
bool Gradeable::hasCorrespondence(const std::string &id) const {
  /*
  for (std::map<std::string,std::pair<int,std::string> >::const_iterator itr = correspondences.begin();
       itr != correspondences.end(); itr++) {
    std::cout << "looking for " << id << " " << itr->first << std::endl;
  }
  */
  std::map<std::string,std::pair<int,std::string> >::const_iterator itr =  correspondences.find(id);
  return (itr != correspondences.end());
}
const std::pair<int, std::string> &
Gradeable::getCorrespondence(const std::string &id) {
  assert (hasCorrespondence(id));
  return correspondences.find(id)->second;
}
bool Gradeable::isReleased(const std::string &id) const {
  assert (released.find(id) != released.end());
  return released.find(id)->second;
}
float Gradeable::getItemMaximum(const std::string &id) const {
  if (maximums.find(id) == maximums.end()){
    return 0;
  }
  return maximums.find(id)->second;
}
float Gradeable::getScaleMaximum(const std::string &id) const {
  if (scale_maximums.find(id) == scale_maximums.end()) {
    return -1;
  }
  return scale_maximums.find(id)->second;
}
float Gradeable::getItemPercentage(const std::string &id) const {
  if (item_percentages.find(id) == item_percentages.end())
    return -1;
  else
    return item_percentages.find(id)->second;
}
float Gradeable::getClamp(const std::string &id) const {
  assert (clamps.find(id) != clamps.end());
  return clamps.find(id)->second;
}
float Gradeable::getSortedWeight(unsigned int position) {
  assert(position < sorted_weights.size());
  return sorted_weights[position];
}
bool Gradeable::hasSortedWeight() {
  return !sorted_weights.empty();
}
void Gradeable::setRemoveLowest(int r) { remove_lowest=r; }
int Gradeable::setCorrespondence(const std::string &id) {
  assert (!hasCorrespondence(id));
  //std::cout << "SET CORR " << id << std::endl;
  assert (int(correspondences.size()) < count);
  int index = correspondences.size();
  correspondences[id] = std::make_pair(index,"");
  return index;
}
void Gradeable::setBucketPercentageUpperClamp(
    float bucket_percentage_upper_clamp) {
  this->bucket_percentage_upper_clamp = bucket_percentage_upper_clamp;
}
void Gradeable::setCorrespondenceName(const std::string &id,
                                      const std::string &name) {
  assert (hasCorrespondence(id));
  assert (correspondences[id].second == "");
  correspondences[id].second = name;
}
void Gradeable::setReleased(const std::string &id, bool is_released) {
  assert (hasCorrespondence(id));
  assert (released.find(id) == released.end());
  released[id] = is_released;
}
void Gradeable::setMaximum(const std::string &id, float maximum) {
  assert (hasCorrespondence(id));
  assert (maximums.find(id) == maximums.end());
  maximums[id] = maximum;
}
void Gradeable::setScaleMaximum(const std::string &id, float scale_maximum) {
  assert (hasCorrespondence(id));
  assert (scale_maximums.find(id) == scale_maximums.end());
  scale_maximums[id] = scale_maximum;
}
void Gradeable::setItemPercentage(const std::string &id,
                                  float item_percentage) {
  assert (hasCorrespondence(id));
  assert (item_percentages.find(id) == item_percentages.end());
  item_percentages[id] = item_percentage;
}
void Gradeable::setClamp(const std::string &id, float clamp) {
  assert (hasCorrespondence(id));
  assert (clamps.find(id) == clamps.end());
  clamps[id] = clamp;
}
void Gradeable::setResubmissionValues(const std::string &id,
                                      const std::string &original_id,
                                      const std::string &resubmit_id,
                                      const std::string &title,
                                      float autograde_replacement_percentage) {
  setCorrespondenceName(id,title);
  original_ids[id] = original_id;
  resubmit_ids[id] = resubmit_id;
  autograde_replacement_percentages[id] = autograde_replacement_percentage;
}
void Gradeable::addSortedWeight(float weight) {
  sorted_weights.push_back(weight);
}

void LookupGradeable(const std::string &id,
                     GRADEABLE_ENUM &g_e, int &i) {
  for (std::size_t k = 0; k < ALL_GRADEABLES.size(); k++) {
    GRADEABLE_ENUM e = ALL_GRADEABLES[k];
    Gradeable g = GRADEABLES[e];
    if (g.hasCorrespondence(id)) {
      g_e = e;
      i = g.getCorrespondence(id).first;
      return;
    }
  }
  assert (0);
}
