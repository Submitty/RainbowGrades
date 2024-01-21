#include <catch2/catch_session.hpp>
#include "gradeable.h"
#include "student.h"

// setup global variables that are needed for the tests
std::string GLOBAL_sort_order;
int GLOBAL_ACTIVE_TEST_ZONE = 0;
std::string GLOBAL_ACTIVE_TEST_ID = "";
std::string GLOBAL_recommend_id = "";
std::vector<std::string> OMIT_SECTION_FROM_STATS;
//====================================================================
// DIRECTORIES & FILES
std::string OUTPUT_FILE                       = "./output.html";
std::string OUTPUT_CSV_FILE                   = "./output.csv";
std::string CUSTOMIZATION_FILE                = "./customization_no_comments.json";
std::string RAW_DATA_DIRECTORY                = "./raw_data/all_grades/";
std::string INDIVIDUAL_FILES_OUTPUT_DIRECTORY = "./individual_summary_html/";
std::string ALL_STUDENTS_OUTPUT_DIRECTORY     = "./all_students_summary_html/";
std::string ALL_STUDENTS_OUTPUT_DIRECTORY_CSV     = "./all_students_summary_csv/";

//====================================================================
// INFO ABOUT GRADING FOR COURSE

std::vector<GRADEABLE_ENUM> ALL_GRADEABLES;

std::map<GRADEABLE_ENUM, GradeableList>  GRADEABLES;

float LATE_DAY_PERCENTAGE_PENALTY = 0;
bool  TEST_IMPROVEMENT_AVERAGING_ADJUSTMENT = false;
bool  LOWEST_TEST_COUNTS_HALF = false;

int QUIZ_NORMALIZE_AND_DROP = 0;

std::map<std::string,float> CUTOFFS;
std::map<GRADEABLE_ENUM,float> OVERALL_FAIL_CUTOFFS;

std::map<Grade,int> grade_counts;
std::map<Grade,float> grade_avg;
int took_final = 0;
int auditors = 0;
int dropped = 0;

Student* PERFECT_STUDENT_POINTER;
Student* AVERAGE_STUDENT_POINTER;
Student* STDDEV_STUDENT_POINTER;

//====================================================================
// INFO ABOUT NUMBER OF SECTIONS

std::map<std::string,std::string> sectionNames;
std::map<std::string,std::string> sectionColors;
//====================================================================

std::string GLOBAL_EXAM_TITLE = "exam title uninitialized";
std::string GLOBAL_EXAM_DATE = "exam date uninitialized";
std::string GLOBAL_EXAM_TIME = "exam time uninitialized";
std::string GLOBAL_EXAM_DEFAULT_ROOM = "exam default room uninitialized";
std::string GLOBAL_EXAM_DEFAULT_BUILDING = "exam default building uninitialized";
std::string GLOBAL_EXAM_SEATING = "";
std::string GLOBAL_SEATING_SPACING = "";
std::string GLOBAL_EXAM_SEATING_COUNT = "";
std::string GLOBAL_LEFT_RIGHT_HANDEDNESS = "";

float GLOBAL_MIN_OVERALL_FOR_ZONE_ASSIGNMENT = 0.1;

int BONUS_WHICH_LECTURE = -1;
std::string BONUS_FILE;

//====================================================================
// INFO ABOUT OUTPUT FORMATTING

bool DISPLAY_INSTRUCTOR_NOTES = false;
bool DISPLAY_EXAM_SEATING = false;
bool DISPLAY_ACADEMIC_SANCTION_DETAILS = false;
bool DISPLAY_FINAL_GRADE = false;
bool DISPLAY_GRADE_SUMMARY = false;
bool DISPLAY_GRADE_DETAILS = false;
bool DISPLAY_LATE_DAYS = false;
bool DISPLAY_RANK_TO_INDIVIDUAL = false;
std::vector<std::string> MESSAGES;
nlohmann::json GLOBAL_CUSTOMIZATION_JSON;

int main( int argc, char* argv[] ) {
  // your setup ...

  int result = Catch::Session().run( argc, argv );

  // your clean-up...

  return result;
}