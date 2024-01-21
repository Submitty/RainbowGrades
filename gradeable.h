#ifndef _GRADEABLE_H_
#define _GRADEABLE_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cassert>


enum class GRADEABLE_ENUM {
  HOMEWORK, ASSIGNMENT, PROBLEM_SET,
    QUIZ, TEST, EXAM,
    EXERCISE, LECTURE_EXERCISE, READING, WORKSHEET, LAB, RECITATION,
    PROJECT, PARTICIPATION, NOTE,
    NONE };

std::string gradeable_enum_to_string(const GRADEABLE_ENUM &g);

bool string_to_gradeable_enum(const std::string &s, GRADEABLE_ENUM &return_value);

// ===============================================================================

// Gradeable Type
struct GradeableID {
public:
  GradeableID() = default;
  explicit GradeableID(std::string id) : value_(std::move(id)) {}
  GradeableID(const GradeableID &) = default;
  GradeableID &operator=(const GradeableID &) = default;
  GradeableID(GradeableID &&) = default;
  GradeableID &operator=(GradeableID &&) = default;
  ~GradeableID() = default;
  const std::string &value() const { return value_; }
  std::string &value() { return value_; }

  friend bool operator==(const GradeableID &a, const GradeableID &b) {
    return a.value_ == b.value_;
  }
  friend bool operator==(const GradeableID &a, const std::string &b) {
    return a.value_ == b;
  }
  friend bool operator==(const std::string &a, const GradeableID &b) {
    return a == b.value_;
  }
  friend bool operator<(const GradeableID &a, const GradeableID &b) {
    return a.value_ < b.value_;
  }

private:
  std::string value_;
};
// gradeable index strong type
struct GradeableIndex {
public:
  GradeableIndex() = default;
  explicit GradeableIndex(size_t index) : value_(index) {}
  GradeableIndex(const GradeableIndex &) = default;
  GradeableIndex &operator=(const GradeableIndex &) = default;
  GradeableIndex(GradeableIndex &&) = default;
  GradeableIndex &operator=(GradeableIndex &&) = default;
  ~GradeableIndex() = default;

  size_t value() const { return value_; }
  size_t &value() { return value_; }
  friend bool operator==(GradeableIndex a, GradeableIndex b) {
    return a.value_ == b.value_;
  }
  friend bool operator==(GradeableIndex a, size_t b) {
    return a.value_ == b;
  }
  friend bool operator==(size_t &a, GradeableIndex b) {
    return a == b.value_;
  }
  friend bool operator<(GradeableIndex a, GradeableIndex b) {
    return a.value_ < b.value_;
  }

private:
  size_t value_;
};

struct Correspondence {

  GradeableIndex index;
  std::string name;

  friend bool operator==(const Correspondence &a, const Correspondence &b) {
    return a.index == b.index;
  }
  friend bool operator<(const Correspondence &a, const Correspondence &b) {
    return a.index < b.index;
  }
};


class GradeableList {

public:

  // CONSTRUTORS
  GradeableList() { count=0;percent=0;remove_lowest=0; }
  GradeableList(int c, float p) : count(c),percent(p) { remove_lowest=0; }

  // ACCESSORS
  int getCount() const;
  float getPercent() const;
  float getBucketPercentageUpperClamp() const;
  float getMaximum() const;
  int getRemoveLowest() const;
  GradeableID getID(GradeableIndex index) const;
  bool hasCorrespondence(const GradeableID &id) const;
  const Correspondence& getCorrespondence(const GradeableID& id) const;
  bool isReleased(const GradeableID &id) const;
  float getItemMaximum(const GradeableID &id) const;
  float getScaleMaximum(const GradeableID &id) const;
  float getItemPercentage(const GradeableID &id) const;
  float getClamp(const GradeableID &id) const;
  float getSortedWeight(unsigned int position);
  bool hasSortedWeight();

  // MODIFIERS
  void setRemoveLowest(int r);
  GradeableIndex setCorrespondence(const GradeableID& id);
  // Set the max percentage that can be received for a gradeable type.
  // If the value is less than 0, it should be ignored.
  void setBucketPercentageUpperClamp(float bucket_percentage_upper_clamp);
  void setCorrespondenceName(const GradeableID& id, const std::string& name);
  void setReleased(const GradeableID& id, bool is_released);
  void setMaximum(const GradeableID &id, float maximum);
  void setScaleMaximum(const GradeableID&id, float scale_maximum);
  void setItemPercentage(const GradeableID&id, float item_percentage);
  void setClamp(const GradeableID& id, float clamp);
  void addSortedWeight(float weight);

private:

  // REPRESENTATION
  int count;
  float percent;
  int remove_lowest;
  float bucket_percentage_upper_clamp;
  // correspondences are a map from the id which corresponds to a gradeables
  // unique id (name) to a pair containing the index of the gradeable along
  // with it's human readable name or "title"
  std::map<GradeableID,Correspondence> correspondences;
  std::map<GradeableID,float> maximums;
  std::map<GradeableID,float> scale_maximums;
  std::map<GradeableID,float> item_percentages;
  std::vector<float> sorted_weights;
  std::map<GradeableID,float> clamps;
  std::map<GradeableID,bool> released;
};

// ===============================================================================

extern std::vector<GRADEABLE_ENUM>    ALL_GRADEABLES;

extern std::map<GRADEABLE_ENUM, GradeableList>  GRADEABLES;


void LookupGradeable(const GradeableID &id,
                     GRADEABLE_ENUM &g_e, GradeableIndex &i);

#endif
