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

struct Gradeable {
  Correspondence correspondence;
  float maximum{0};
  float scale_maximum{-1};
  float item_percentage{-1};
  float clamp{0};
  bool released{false};

  /*
   * Returns true if this gradeable is extra credit.
   * Extra credit items are those that have a maximum of <= 0.
   */
  [[nodiscard]] bool isExtraCredit() const { return maximum <= 0;}
  /*
   * returns value that should be used as the maximum for the gradeable.
   */
  [[nodiscard]] float getScaledMaximum() const {return std::max(maximum,scale_maximum);}

};

class GradeableList {

public:

  // CONSTRUTORS
  GradeableList() { count=0;percent=0;remove_lowest=0; }
  GradeableList(int c, float p) : count(c),percent(p) { remove_lowest=0; }

  // ACCESSORS
  [[nodiscard]] size_t getCount() const;
  [[nodiscard]] float getPercent() const;
  [[nodiscard]] float getBucketPercentageUpperClamp() const;

  /*
   * Get the maximum percentage that can be received for a gradeable type.
   * If the value is less than 0, it should be ignored.
   */
  [[nodiscard]] float getExpectedTotalPoints() const;
  [[nodiscard]] int getRemoveLowest() const;
  [[nodiscard]] float getSortedWeight(unsigned int position);
  [[nodiscard]] bool hasSortedWeight();

  [[nodiscard]] Gradeable& getGradeable(const GradeableID& id);
  [[nodiscard]] const Gradeable& getGradeable(const GradeableID& id) const;
  
  // functions that operate on gradeable
  [[nodiscard]] GradeableID getID(GradeableIndex index) const;
  [[nodiscard]] bool hasCorrespondence(const GradeableID &id) const;
  [[nodiscard]] const Correspondence& getCorrespondence(const GradeableID& id) const;
  [[nodiscard]] bool isReleased(const GradeableID &id) const;
  [[nodiscard]] float getItemMaximum(const GradeableID &id) const;
  [[nodiscard]] float getScaleMaximum(const GradeableID &id) const;
  [[nodiscard]] float getItemPercentage(const GradeableID &id) const;
  [[nodiscard]] float getClamp(const GradeableID &id) const;

  /*
   * Get the number of items that are extra credit.
   * Extra credit items are those that have a maximum of <= 0.
   */
  [[nodiscard]] size_t getExtraCreditCount() const;
  /*
   * Get the number of items that are not extra credit.
   * Non-extra credit items are those that have a maximum of > 0.
   */
  [[nodiscard]] size_t getNormalCount() const;

  /*
   * Get the number of points that are extra credit
   */
  [[nodiscard]] float getExtraCreditPoints() const;

  /*
   * Get the number of points that are not extra credit
   */
  [[nodiscard]] float getNormalPoints() const;

  /*
   * Get the total number of points
   */
  [[nodiscard]] float getTotalPoints() const;

  size_t getZeroMaxCount() const;
  size_t getNonExtraCreditCount() const;
  size_t getNonZeroCount() const;
  float getNonZeroSum() const;


  // MODIFIERS
  void setRemoveLowest(int r);
  void addSortedWeight(float weight);
  // Set the max percentage that can be received for a gradeable type.
  // If the value is less than 0, it should be ignored.
  void setBucketPercentageUpperClamp(float bucket_percentage_upper_clamp);
  
  // functions that operate on gradeable
  GradeableIndex setCorrespondence(const GradeableID& id);
  void setCorrespondenceName(const GradeableID& id, const std::string& name);
  void setReleased(const GradeableID& id, bool is_released);
  void setMaximum(const GradeableID &id, float maximum);
  void setScaleMaximum(const GradeableID&id, float scale_maximum);
  void setItemPercentage(const GradeableID&id, float item_percentage);
  void setClamp(const GradeableID& id, float clamp);

private:

  // REPRESENTATION
  size_t count;
  float percent;
  int remove_lowest;
  float bucket_percentage_upper_clamp;
  std::vector<float> sorted_weights;

  std::map<GradeableID, Gradeable> gradeables_;

  /*
  // correspondences are a map from the id which corresponds to a gradeables
  // unique id (name) to a pair containing the index of the gradeable along
  // with it's human readable name or "title"
  std::map<GradeableID,Correspondence> correspondences;
  std::map<GradeableID,float> maximums;
  std::map<GradeableID,float> scale_maximums;
  std::map<GradeableID,float> item_percentages;
  std::map<GradeableID,float> clamps;
  std::map<GradeableID,bool> released;
  */
};

// ===============================================================================

extern std::vector<GRADEABLE_ENUM>    ALL_GRADEABLES;

extern std::map<GRADEABLE_ENUM, GradeableList>  GRADEABLES;


void LookupGradeable(const GradeableID &id,
                     GRADEABLE_ENUM &g_e, GradeableIndex &i);

#endif
