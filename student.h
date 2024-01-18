#ifndef _STUDENT_H_
#define _STUDENT_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cassert>
#include <map>
#include <algorithm>
#include <sstream>

#include "gradeable.h"
#include "constants_and_globals.h"

extern std::vector<float> GLOBAL_earned_late_days;

//====================================================================
//====================================================================
// stores the grade for a single gradeable

class ItemGrade {
public:
  ItemGrade(float v, int ldu=0, const std::string& n="", const std::string &s="", const std::string &e="", bool ai=false, int de=0, const std::string& r="") {
    value = v;
    late_days_used = ldu;
    note = n;
    event = e;
    academic_integrity = ai;
    late_day_exceptions = de;
    reason_for_exception = r;
    
    if (s != "UNKONWN") {
      status = s;
    }
  }
  float getValue() const { 

    float adjusted_value = value;
    if (late_days_used > 0) {
      // FIXME:  Currently a flat penalty no matter how many days used
      adjusted_value = (1-LATE_DAY_PERCENTAGE_PENALTY)*value;
    }
    
    return adjusted_value;
  }
  int getLateDaysUsed() const { return late_days_used; }
  int getLateDayExceptions() const { return late_day_exceptions; }
  const std::string& getReasonForException() const { return reason_for_exception; }
  const std::string& getNote() const { return note; }
  const std::string& getStatus() const { return status; }
  const std::string& getEvent() const { return event; }
  bool getAcademicIntegrity() const { return academic_integrity; }
    
private:
  float value;
  int late_days_used;
  int late_day_exceptions;
  bool academic_integrity;
  std::string note;
  std::string status;
  std::string event;
  std::string reason_for_exception;
};

//====================================================================
//====================================================================

class Student {

public:

  // ---------------
  // CONSTRUCTOR
  Student();

  // ---------------
  // ACCESSORS

  // personal data
  const std::string& getUserName()      const { return username; }
  const std::string& getFirstName()     const { return legal_first; }
  const std::string& getPreferredFirstName() const { if (preferred_first != "") return preferred_first; return legal_first; }
  const std::string& getLastName()      const { return legal_last; }
  const std::string& getPreferredLastName()      const { if (preferred_last != "") return preferred_last; return legal_last; }
  const std::string& getLastUpdate()    const { return lastUpdate; }
  bool getLefty() const { return lefty; }
  
  // registration status
  const std::string& getSection()           const { return section; }
  const std::string& getCourseSectionId()           const { return course_section_id; }
  int getRotatingSection()   const { return rotating_section; }
  bool getAudit()            const { return audit; }
  bool getWithdraw()         const { return withdraw; }
  bool getIndependentStudy() const { return independentstudy; }

  // grade data
  const ItemGrade& getGradeableItemGrade(GRADEABLE_ENUM g, int i) const;
  std::string getZone(int i) const;
  int getAllowedLateDays(int which_lecture) const;
  int getPollsCorrect() const;
  int getPollsIncorrect() const;
  float getPollPoints() const;
  int getUsedLateDays() const;
  int getLateDayExceptions() const;
  std::vector<std::tuple<ItemGrade,std::tuple<GRADEABLE_ENUM,int> > > getItemsWithExceptions() const;
  float getAcademicSanctionPenalty() const { return academic_sanction_penalty; }

  void setCurrentAllowedLateDays(int d) { current_allowed_late_days = d; }
  void setDefaultAllowedLateDays(int d) { default_allowed_late_days = d; }

  int getDefaultAllowedLateDays() const { return default_allowed_late_days; }

  void add_bonus_late_day(int which_lecture) {
    //std::cout << "ADD BONUS " << which_lecture << " " << username << std::endl;
    bonus_late_days_which_lecture.push_back(which_lecture);
  }
  bool get_bonus_late_day(int which_lecture) const {
    for (unsigned int i = 0; i < bonus_late_days_which_lecture.size(); i++) {
      if (bonus_late_days_which_lecture[i] == which_lecture) {
        //std::cout << "YES BONUS " << which_lecture << " " << username << std::endl;
        return true;
      }
    }
    return false;
  }

  // other grade-like data
  const std::string& getNumericID() const { return numeric_id; }
  bool getAcademicIntegrityForm()  const { return academic_integrity_form; }
  float getParticipation()           const { return participation; }
  float getUnderstanding()           const { return understanding; }

  // info about exam assignments
  const std::string& getExamRoom() const { return exam_room; }
  const std::string& getExamBuilding() const { return exam_building; }
  const std::string& getExamZone() const { return exam_zone; }
  std::string getExamRow() const { return exam_row;}
  std::string getExamSeat() const { return exam_seat;}
  const std::string& getExamTime() const { return exam_time; }
  const std::string& getExamZoneImage() const { return exam_zone_image; }
  int getRank() const { return rank; }

  // per student notes
  const std::string& getRecommendation()          const { return ta_recommendation; }
  const std::string& getOtherNote()                  const { return other_note; }
  const std::vector<std::string>& getEarlyWarnings() const { return early_warnings; }

  // ---------------
  // MODIFIERS

  // personal data
  void setUserName(const std::string &s)      { username=s; }
  void setLegalFirstName(const std::string &s)     { legal_first=s; }
  void setPreferredFirstName(const std::string &s) { preferred_first=s;  }
  void setPreferredLastName(const std::string &s) { preferred_last=s;  }
  void setLegalLastName(const std::string &s)      { legal_last=s; }
  void setLefty() { lefty = true; }
  void setLastUpdate(const std::string &s)    { lastUpdate = s; }

  // registration status
  void setSection(std::string x) { section = x; }
  void setCourseSectionId(const std::string &x) { course_section_id = x; }
  void setRotatingSection(int x) { rotating_section = x; }
  void setAudit() { audit = true; }
  void setWithdraw() { withdraw = true; }
  void setIndependentStudy() { independentstudy = true; }

  // grade data
  void setTestZone(int which_test, const std::string &zone)  { zones[which_test] = zone; }
  void setGradeableItemGrade(GRADEABLE_ENUM g, int i, float value, int late_days_used=0, const std::string &note="",const std::string &status="");
  void setGradeableItemGrade_AcademicIntegrity(GRADEABLE_ENUM g, int i, float value, bool academic_integrity, int late_days_used=0, const std::string &note="",const std::string &status="");
  void setGradeableItemGrade_border(GRADEABLE_ENUM g, int i, float value, const std::string &event="", int late_days_used=0, const std::string &note="",const std::string &status="",int exceptions=0, const std::string &reason="");

  void academic_sanction(const std::string &gradeable, float penalty);

   //set in order of priority - top to bottom
    void set_event_academic_integrity(bool value) {academic_integrity = value;}
    void set_event_overridden(bool value) {overridden = value;}
    void set_event_extension(bool value) {extension = value;}
    void set_event_grade_inquiry(bool value) {grade_inquiry = value;}
    void set_event_cancelled(bool value) {cancelled = value;}
    void set_event_version_conflict(bool value) {version_conflict = value;}
    void set_event_bad_status(bool value) {bad_status = value;}

    //bool in order of priority - top to bottom
    bool get_event_academic_integrity() {return academic_integrity;}
    bool get_event_overridden() {return overridden;}
    bool get_event_extension() {return extension;}
    bool get_event_grade_inquiry() {return grade_inquiry;}
    bool get_event_cancelled() {return cancelled;}
    bool get_event_version_conflict() {return version_conflict;}
    bool get_event_bad_status() {return bad_status;}

  // other grade-like data
  void setNumericID(const std::string& r_id) { numeric_id = r_id; }
  void setAcademicIntegrityForm() { academic_integrity_form = true; }
  void setParticipation(float x) { participation = x; }
  void setUnderstanding(float x) { understanding = x; }
  void setRank(int x) { rank = x; }

  // info about exam assignments
  void setExamRoom(const std::string &s) { exam_room = s; }
  void setExamBuilding(const std::string &s) { exam_building = s; }
  void setExamZone(const std::string &z, const std::string &r, const std::string &s) { exam_zone=z; exam_row=r; exam_seat=s; }
  void setExamTime(const std::string &s) { exam_time = s; }
  void setExamZoneImage(const std::string &s) { exam_zone_image = s; }

  // per student notes
  void addWarning(const std::string &message) { early_warnings.push_back(message); }
  void addRecommendation(const std::string &message) { ta_recommendation += message; }
  void addNote(const std::string &message) { other_note += message; }
  void ManualGrade(const std::string &grade, const std::string &message);


  // ---------------
  // POLLS

  void incrementPollsCorrect(unsigned int amount);
  void incrementPollsIncorrect(unsigned int amount);


  // HELPER FUNCTIONS
  float GradeablePercent(GRADEABLE_ENUM g) const;
  float overall() const { return overall_b4_academic_sanction() + academic_sanction_penalty; }
  float adjusted_test(int i) const;
  float adjusted_test_pct() const;
  float lowest_test_counts_half_pct() const;
  float quiz_normalize_and_drop(int num) const;
  float overall_b4_academic_sanction() const;
  std::string grade(bool flag_b4_academic_sanction, Student *lowest_d) const;
  void outputgrade(std::ostream &ostr,bool flag_b4_academic_sanction,Student *lowest_d) const;
  
private:

  // ---------------
  // REPRESENTATION

  // personal data
  std::string username;
  std::string legal_first;
  std::string preferred_first;
  std::string legal_last;
  std::string preferred_last;
  bool lefty;
  
  std::string lastUpdate;

  int current_allowed_late_days;
  int default_allowed_late_days;
  bool academic_integrity = false;
  bool overridden = false;
  bool extension = false;
  bool grade_inquiry = false;
  bool version_conflict = false;
  bool cancelled = false;
  bool bad_status = false;

    // registration status
  std::string section;
  std::string course_section_id;
  int rotating_section;
  bool audit;
  bool withdraw;
  bool independentstudy;

  // grade data
  std::map<GRADEABLE_ENUM,std::vector<ItemGrade> > all_item_grades;
  
  std::vector<std::string> zones;
  float academic_sanction_penalty;
  float cached_hw;
  int rank;

  // other grade-like data
  std::string numeric_id;
  bool academic_integrity_form;
  float participation;
  float understanding;

  std::vector<int> bonus_late_days_which_lecture;

  // info about exam assignments
  std::string exam_zone;
  std::string exam_row;
  std::string exam_seat;
  std::string exam_room;
  std::string exam_building;
  std::string exam_time;
  std::string exam_zone_image;

  // per student notes 
  std::string ta_recommendation;
  std::string other_note;
  std::string manual_grade;
  std::vector<std::string> early_warnings;

  // Polls and late days
  int polls_correct;
  int polls_incorrect;
  bool earn_late_days_from_polls;
};

//====================================================================
//====================================================================

Student* GetStudent(const std::vector<Student*> &students, const std::string& username);

#endif
