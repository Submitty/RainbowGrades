#include "student.h"

extern std::map<GRADEABLE_ENUM,float> OVERALL_FAIL_CUTOFFS;
const std::string GradeColor(const std::string &grade);
std::vector<float> GLOBAL_earned_late_days;

// =============================================================================================
// =============================================================================================
// CONSTRUCTOR

Student::Student() { 

  // personal data
  // (defaults to empty string)
  lefty = false;
  
  // registration status
  section = "null";  
  audit = false;
  withdraw = false;
  independentstudy = false;

  default_allowed_late_days = 0;
  current_allowed_late_days = 0;
  polls_correct = 0;
  polls_incorrect = 0;

  // grade data
  for (unsigned int i = 0; i < ALL_GRADEABLES.size(); i++) { 
    GRADEABLE_ENUM g = ALL_GRADEABLES[i];
    all_item_grades[g]   = std::vector<ItemGrade>(GRADEABLES[g].getCount(),ItemGrade(0));
  }

  earn_late_days_from_polls = true; //TODO: Change this to false, make customization.json option

  rotating_section = -1;
  zones = std::vector<std::string>(GRADEABLES[GRADEABLE_ENUM::TEST].getCount(),"");
  academic_sanction_penalty = 0;
  cached_hw = -1;

  // other grade-like data
  numeric_id = "";
  academic_integrity_form = false;
  participation = 0;
  understanding = 0;

  // info about exam assignments
  // (defaults to empty string)

  // per student notes
  // (defaults to empty string)
}




// lookup a student by username
Student* GetStudent(const std::vector<Student*> &students, const std::string& username) {
  for (unsigned int i = 0; i < students.size(); i++) {
    if (students[i]->getUserName() == username) return students[i];
  }
  return NULL;
}





// =============================================================================================
// =============================================================================================
// accessor & modifier for grade data

const ItemGrade& Student::getGradeableItemGrade(GRADEABLE_ENUM g, int i) const {
  static ItemGrade emptyItemGrade(0);
  //std::cout << "i " << i << "   count " << GRADEABLES[g].getCount() << std::endl;
  if (i >= GRADEABLES[g].getCount()) {
    return emptyItemGrade;
  }
  assert (i >= 0 && i < GRADEABLES[g].getCount());
  std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::const_iterator itr = all_item_grades.find(g);
  assert (itr != all_item_grades.end());
  assert (int(itr->second.size()) > i);
  
  return itr->second[i]; //return value; 
}



void Student::setGradeableItemGrade(GRADEABLE_ENUM g, int i, float value, 
                                    int late_days_used, const std::string &note, const std::string &status) {
  assert (i >= 0 && i < GRADEABLES[g].getCount());
  std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::iterator itr = all_item_grades.find(g);
  assert (itr != all_item_grades.end());
  assert (int(itr->second.size()) > i);
  itr->second[i] = ItemGrade(value,late_days_used,note,status);
}

void Student::setGradeableItemGrade_AcademicIntegrity(GRADEABLE_ENUM g, int i, float value, bool academic_integrity,
                                    int late_days_used, const std::string &note, const std::string &status) {
  assert (i >= 0 && i < GRADEABLES[g].getCount());
  std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::iterator itr = all_item_grades.find(g);
  assert (itr != all_item_grades.end());
  assert (int(itr->second.size()) > i);
  std::string temp = "";
    
  itr->second[i] = ItemGrade(value,late_days_used,note,status,temp,academic_integrity);
}

void Student::setGradeableItemGrade_border(GRADEABLE_ENUM g, int i, float value, const std::string &event, 
                                           int late_days_used, const std::string &note, const std::string &status, int exceptions, const std::string &reason) {
  assert (i >= 0 && i < GRADEABLES[g].getCount());
  std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::iterator itr = all_item_grades.find(g);
  assert (itr != all_item_grades.end());
  assert (int(itr->second.size()) > i);
  bool academic_integrity = false;
    
  itr->second[i] = ItemGrade(value,late_days_used,note,status,event,academic_integrity,exceptions,reason);
}

float Student::GradeablePercent(GRADEABLE_ENUM gradeable_category) const {
  auto & gradeable = GRADEABLES[gradeable_category];
  if (gradeable.getCount() == 0) return 0;
  assert (gradeable.getPercent() >= 0);

  // special rules for tests
  if (gradeable_category == GRADEABLE_ENUM::TEST && TEST_IMPROVEMENT_AVERAGING_ADJUSTMENT) {
    return adjusted_test_pct();
  }

  // normalize & drop lowest #
  if (gradeable_category == GRADEABLE_ENUM::QUIZ && QUIZ_NORMALIZE_AND_DROP > 0) {
    return quiz_normalize_and_drop(QUIZ_NORMALIZE_AND_DROP);
  }

  if (gradeable_category == GRADEABLE_ENUM::TEST && LOWEST_TEST_COUNTS_HALF) {
    return lowest_test_counts_half_pct();
  }

  //Do one pass to get the defined item scores
  float nonzero_sum = 0;
  int nonzero_count = 0;
  int non_extra_credit_count = 0;

  getNonzeroCounts(gradeable, nonzero_sum, nonzero_count,
                   non_extra_credit_count);

  // If there are no gradeables with a max >0, bucket is 0% anyway
  if(nonzero_count == 0){
    return 0.0;
  }

  // collect the scores in a vector
  std::vector<score_object> scores = fillScoreVector(
      gradeable_category, gradeable, nonzero_sum, nonzero_count);
  float percentage =
      calculateScorePercentages(gradeable, non_extra_credit_count, scores);

  return 100 * percentage;
}
std::vector<score_object>
Student::fillScoreVector(GRADEABLE_ENUM &gradeable_category,
                         const Gradeable &gradeable, float nonzero_sum,
                         int nonzero_count) const {
  std::vector<score_object> scores;
  for (int i = 0; i < gradeable.getCount(); i++) {
    float item_grade = getGradeableItemGrade(gradeable_category,i).getValue();
    std::string id = gradeable.getID(i);
    // this is used for extra credit gradeables to get the maximum score
    float item_maximum = nonzero_sum/nonzero_count;
    if(!id.empty()){
      item_maximum = gradeable.getItemMaximum(id);
    }
    float item_percentage = gradeable.getItemPercentage(id);
    float gradeable_scale_maximum = gradeable.getScaleMaximum(id);
    scores.push_back(score_object(item_grade, item_maximum, item_percentage, gradeable_scale_maximum));
  }
  return scores;
}
float calculateScorePercentages(Gradeable &gradeable,
                                         int non_extra_credit_count,
                                         std::vector<score_object> &scores)
{
  // sort the scores (smallest first)
  std::sort(scores.begin(),scores.end());
  // to check that the number of "drop the lowest" is less than the number of
  // non extra credit gradeables,
  // i.e., it is not allowed to drop all non extra credit gradeables
  assert(gradeable.getRemoveLowest() >= 0 &&
             ((non_extra_credit_count > 0 &&
               gradeable.getRemoveLowest() < non_extra_credit_count)) ||
         (gradeable.getRemoveLowest() == 0));

  // sum the remaining (higher) scores
  float sum_max = 0;
  for (int i = gradeable.getRemoveLowest(); i < gradeable.getCount(); i++) {
    float m = scores[i].max;
    sum_max += m;
  }

  float sum_scaled_max = 0;
  for (int i = gradeable.getRemoveLowest(); i < gradeable.getCount(); i++) {
    if(scores[i].max > 0){
      continue;
    }
    float m = scores[i].scale_max;
    sum_scaled_max += m;
  }

  // sum the remaining (higher) scores
  float sum = 0;
  // to also sum the remaining (higher) percentages
  float sum_percentage = 0;
  for (int i = gradeable.getRemoveLowest(); i < gradeable.getCount(); i++) {
    float s = scores[i].score;
    float m = scores[i].max;
    float p = scores[i].percentage;
    float sm = scores[i].scale_max;
    float my_max = std::max(m,sm);

    if (p < 0) {
      if (sum_max > 0) {
        p = std::max(m,sm) / sum_max;
        if(gradeable.hasSortedWeight()){
          p = gradeable.getSortedWeight(i);
        }
      } else {
        // pure extra credit category
        p = std::max(m,sm) / sum_scaled_max;
      }
    }
    if (my_max > 0) {
      sum += p * s / my_max;
    }
    //   to add percentages only for non-extra credit gradeables
    if (m != 0.0) {
      sum_percentage += p;
    }
  }
  const float tolerance = 0.000001;
  assert(sum_percentage <= 1.0 + tolerance);
  if (sum_max == 0) { // pure extra credit category
    sum_percentage = 1.0;
  }

  float percentage = gradeable.hasSortedWeight() ? sum : gradeable.getPercent() * sum / sum_percentage;
  float percentage_upper_clamp = gradeable.getBucketPercentageUpperClamp();
  if (percentage_upper_clamp > 0) {
    percentage = std::min(percentage, percentage_upper_clamp);
  }
  return percentage;
}
void getNonzeroCounts(const Gradeable &gradeable, float &nonzero_sum,
                               int &nonzero_count,
                               int &non_extra_credit_count) {
  for (int i = 0; i < gradeable.getCount(); i++) {
    //float s = getGradeableItemGrade(gradeable_category,i).getValue();
    std::string id = gradeable.getID(i);
    if(!id.empty()){
      if (gradeable.getItemMaximum(id) > 0) {
        non_extra_credit_count++;
      }
      float m = std::max(gradeable.getItemMaximum(id),gradeable.getScaleMaximum(id));
      if(m > 0){
        nonzero_sum += m;
        nonzero_count++;
      }
    }
  }
}

float Student::adjusted_test(int i) const {
  assert (i >= 0 && i <  GRADEABLES[GRADEABLE_ENUM::TEST].getCount());
  float a = getGradeableItemGrade(GRADEABLE_ENUM::TEST,i).getValue();
  float b;
  if (i+1 < GRADEABLES[GRADEABLE_ENUM::TEST].getCount()) {
    b = getGradeableItemGrade(GRADEABLE_ENUM::TEST,i+1).getValue();
  } else {
    assert (GRADEABLES[GRADEABLE_ENUM::EXAM].getCount() == 1);
    b = getGradeableItemGrade(GRADEABLE_ENUM::EXAM,0).getValue();
    // HACK  need to scale the final exam!
    b *= 0.6667;
  }

  if (a > b) return a;
  return (a+b) * 0.5;
}


float Student::adjusted_test_pct() const {
  float sum = 0;
  for (int i = 0; i < GRADEABLES[GRADEABLE_ENUM::TEST].getCount(); i++) {
    sum += adjusted_test(i);
  }
  float answer =  100 * GRADEABLES[GRADEABLE_ENUM::TEST].getPercent() * sum / float (GRADEABLES[GRADEABLE_ENUM::TEST].getMaximum());
  return answer;
}



float Student::quiz_normalize_and_drop(int num) const {

  assert (num > 0);

  // collect the normalized quiz scores in a vector
  std::vector<float> scores;
  for (int i = 0; i < GRADEABLES[GRADEABLE_ENUM::QUIZ].getCount(); i++) {
    // the max for this quiz
    float p = PERFECT_STUDENT_POINTER->getGradeableItemGrade(GRADEABLE_ENUM::QUIZ,i).getValue();
    // this students score
    float v = getGradeableItemGrade(GRADEABLE_ENUM::QUIZ,i).getValue();
    scores.push_back(v/p);
  }

  assert (scores.size() > std::size_t(num)); //Relies on the assert(num > 0) at the top of this function.

  // sort the scores
  sort(scores.begin(),scores.end());

  // sum up all but the lowest "num" scores
  float sum = 0;
  for (std::size_t i = num; i < scores.size(); i++) {
    sum += scores[i];
  }

  // the overall percent of the final grade for quizzes
  return 100 * GRADEABLES[GRADEABLE_ENUM::QUIZ].getPercent() * sum / float (scores.size()-num);
}


float Student::lowest_test_counts_half_pct() const {

  int num_tests = GRADEABLES[GRADEABLE_ENUM::TEST].getCount();
  assert (num_tests > 0);

  // first, collect & sort the scores
  std::vector<float> scores;
  for (int i = 0; i < num_tests; i++) {
    scores.push_back(getGradeableItemGrade(GRADEABLE_ENUM::TEST,i).getValue());
  }
  std::sort(scores.begin(),scores.end());

  // then sum the scores
  float sum = 0.5 * scores[0];
  float weight_total = 0.5;
  for (int i = 1; i < num_tests; i++) {
    sum += scores[i];
    weight_total += 1.0;
  }

  // renormalize!
  sum *= float(num_tests) / weight_total;

  // scale to percent;
  return 100 * GRADEABLES[GRADEABLE_ENUM::TEST].getPercent() * sum / float (GRADEABLES[GRADEABLE_ENUM::TEST].getMaximum());
}


// =============================================================================================
// =============================================================================================

int Student::getAllowedLateDays(int which_lecture) const {
  if (getSection() == "null") return 0;

  //int answer = 2;

  int answer = default_allowed_late_days;

  // average 4 questions per lecture 2-28 ~= 112 clicker questions
  //   30 questions => 3 late days
  //   60 questions => 4 late days
  //   90 qustions  => 5 late days

  float total = 0;
  //TODO: Condition may need to change to use a global depending on how we do it in customization.json
  if(earn_late_days_from_polls){
    total = getPollPoints();
  }

  for (unsigned int i = 0; i < GLOBAL_earned_late_days.size(); i++) {
    if (total >= GLOBAL_earned_late_days[i]) {
      answer++;
    }
  }

  for (unsigned int i = 0; i < bonus_late_days_which_lecture.size(); i++) {
    if (bonus_late_days_which_lecture[i] <= which_lecture) {
      answer++;
    }
  }

  return std::max(current_allowed_late_days,answer);

}

// get the total used late days
int Student::getUsedLateDays() const {
  int answer = 0;
  for (std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::const_iterator itr = all_item_grades.begin(); itr != all_item_grades.end(); itr++) {
    for (std::size_t i = 0; i < itr->second.size(); i++) {
      answer += itr->second[i].getLateDaysUsed();
    }
  }
  return answer;
}

 int Student::getLateDayExceptions() const {
  int answer = 0;
  for (std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::const_iterator itr = all_item_grades.begin(); itr != all_item_grades.end(); itr++) {
    for (std::size_t i = 0; i < itr->second.size(); i++) {
      answer += itr->second[i].getLateDayExceptions();
    }
  }
  return answer;
 }

 std::vector<std::tuple<ItemGrade,std::tuple<GRADEABLE_ENUM,int> > > Student::getItemsWithExceptions() const {
  std::vector<std::tuple<ItemGrade,std::tuple<GRADEABLE_ENUM,int> > > result;
  for (std::map<GRADEABLE_ENUM,std::vector<ItemGrade> >::const_iterator itr = all_item_grades.begin(); itr != all_item_grades.end(); itr++) {
    for (std::size_t i = 0; i < itr->second.size(); i++) {
      if (itr->second[i].getLateDayExceptions()>0) {
        std::tuple<GRADEABLE_ENUM,int> indexes{itr->first,i};
        std::tuple<ItemGrade,std::tuple<GRADEABLE_ENUM,int> > itemInfo{itr->second[i],indexes};
        result.push_back(itemInfo);
      }
    }
  }
  return result;
 }

// =============================================================================================

float Student::overall_b4_academic_sanction() const {
  float answer = 0;
  for (unsigned int i = 0; i < ALL_GRADEABLES.size(); i++) {
    GRADEABLE_ENUM g = ALL_GRADEABLES[i];
    answer += GradeablePercent(g);
  }
  return answer;
}

std::string Student::grade(bool flag_b4_academic_sanction, Student *lowest_d) const {

  if (section == "null") return "";
  if (!flag_b4_academic_sanction && manual_grade != "") return manual_grade;
  float over = overall();
  if (flag_b4_academic_sanction) {
    over = overall_b4_academic_sanction();
  }

  // otherwise apply the cutoffs
  if (over >= CUTOFFS["A"])  return "A";
  if (over >= CUTOFFS["A-"]) return "A-";
  if (over >= CUTOFFS["B+"]) return "B+";
  if (over >= CUTOFFS["B"])  return "B";
  if (over >= CUTOFFS["B-"]) return "B-";
  if (over >= CUTOFFS["C+"]) return "C+";
  if (over >= CUTOFFS["C"])  return "C";
  if (over >= CUTOFFS["C-"]) return "C-";
  if (over >= CUTOFFS["D+"]) return "D+";
  if (over >= CUTOFFS["D"])  return "D";
  else return "F";
}



void Student::academic_sanction(const std::string &gradeable, float penalty) {

  // if the penalty is "a whole or partial letter grade"....
  float average_letter_grade = (CUTOFFS["A"]-CUTOFFS["B"] +
                                 CUTOFFS["B"]-CUTOFFS["C"] +
                                 CUTOFFS["C"]-CUTOFFS["D"]) / 3.0;
  GRADEABLE_ENUM g;
  int item;
  LookupGradeable(gradeable,g,item);
  if (!GRADEABLES[g].hasCorrespondence(gradeable)) {
    std::cerr << "WARNING -- NO GRADEABLE TO ACADEMIC SANCTION" << std::endl;
  } else {
    int which = GRADEABLES[g].getCorrespondence(gradeable).first;
    if (!(getGradeableItemGrade(g,which).getValue() > 0)) {
      std::cerr << "WARNING:  the grade for this " << gradeable_enum_to_string(g)<<" is already 0, academic sanction penalty error?" << std::endl;
    }
      setGradeableItemGrade_AcademicIntegrity(g,which,0, true);
  }

  // the penalty is positive
  // but it will be multiplied by a negative and added to the total;
  assert (penalty >= 0);

  academic_sanction_penalty += -0.0000002;
  academic_sanction_penalty += -average_letter_grade * penalty;
  std::stringstream foo;
  foo << std::setprecision(2) << std::fixed << penalty;

  addWarning("[PLAGIARISM " + gradeable + " " + foo.str() + "]<br>");
}



void Student::ManualGrade(const std::string &grade, const std::string &message) {
  assert (grade == "A" ||
          grade == "A-" ||
          grade == "B+" ||
          grade == "B" ||
          grade == "B-" ||
          grade == "C+" ||
          grade == "C" ||
          grade == "C-" ||
          grade == "D+" ||
          grade == "D" ||
          grade == "F");
  manual_grade = grade;
  other_note += "awarding a " + grade + " because " + message;
}


void Student::outputgrade(std::ostream &ostr,bool flag_b4_academic_sanction,Student *lowest_d) const {
  std::string g = grade(flag_b4_academic_sanction,lowest_d);
  
  std::string color = GradeColor(g);
  if (academic_sanction_penalty < -0.01) {
    ostr << "<td align=center bgcolor=" << color << ">" << g << " @</td>";
  } else {
    ostr << "<td align=center bgcolor=" << color << ">" << g << "</td>";
  }
}


// =============================================================================================

// zones for exams...
//const std::string& Student::getZone(int i) const {
std::string Student::getZone(int i) const {
  assert (i >= 0 && i < GRADEABLES[GRADEABLE_ENUM::TEST].getCount()); return zones[i]; 
}

int Student::getPollsCorrect() const{
  return polls_correct;
}

int Student::getPollsIncorrect() const{
  return polls_incorrect;
}

//TODO: In the future if we allow custom correct/incorrect weights, this calculation will need changing
float Student::getPollPoints() const {
  return float(getPollsCorrect()) + 0.5 * float(getPollsIncorrect());
}

// =============================================================================================

void Student::incrementPollsCorrect(unsigned int amount=1){
  polls_correct += amount;
}

void Student::incrementPollsIncorrect(unsigned int amount=1){
  polls_incorrect += amount;
}

// =============================================================================================
