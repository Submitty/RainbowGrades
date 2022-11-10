#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <ctime>
#include <chrono>

#include <nlohmann/json.hpp>

#include "student.h"
#include "submini_polls.h"

std::map<std::string,std::map<std::string,LectureResult> > GLOBAL_students;
std::map<int,std::pair<std::string,int> > GLOBAL_poll_id_map;
std::map<std::string,std::map<int,Poll> > GLOBAL_lectures;


// Polls are organized into buckets by date
void group_polls_by_date(int poll_id, const std::string &date, std::string &lecture, int &which) {
  which = poll_id;
  lecture = date;
}


// Helper function to print debug information about the poll
std::ostream& operator<<(std::ostream& ostr, const Poll &p) {
  if (p.getCorrectOptions().size() == 0) {
    std::cerr << "ERROR WITH " << p.getName() << std::endl;
  }
  assert (p.getCorrectOptions().size() > 0);
  ostr << "  "
       << std::left
       << std::setw(40) << p.getName() << " "
       << std::setw(15) << p.getDate() << " "
       << p.getCorrectOptions().size() << " " << p.getWrongOptions().size() << "   "
       << std::right << std::setw(3) << p.getCorrectClicks() << " " << std::setw(3) << p.getWrongClicks();
  return ostr;
}



bool earnedlatetoday(int prev, int today) {
  int earn_late[5] = { 15, 45, 75, 105, 135 };
  for (int i = 0; i < 5; i++) {
    if (prev < earn_late[i] && today >= earn_late[i]) return true;
  }
  return false;
}



std::string convert_date(const std::string &date) {
  std::stringstream ss(date);
  int x,y,z;
  char ch,ch2;
  ss >> x >> ch >> y >> ch2 >> z;
  int year, month, day;
  if (ch == '-' && ch2 == '-') {
    // old format
    year = x;
    month = y;
    day = z;
  } else {
    assert (ch == '/' && ch2 == '/');
    // new format
    month = x;
    day = y;
    year = z;
  }
  assert (year > 2000);
  assert (month >= 1 && month <= 12);
  assert (day >= 1 && day <= 31);
  std::stringstream out;
  out << std::setw(2) << std::setfill('0') << month << "-"
      << std::setw(2) << std::setfill('0') << day << "-" << year;
  return out.str();
}

// =======================================================================================
// =======================================================================================
// Load all poll data
void LoadPolls(const std::vector<Student*> &students) {

  // ----------------------------------------------------------------------------
  // initialize a structure with the names of all students in the course
  std::string path = "../raw_data";
  for (unsigned int i = 0; i < students.size(); i++) {
    std::string tmp = students[i]->getUserName();
    std::string section = students[i]->getSection();
    if (section == "null") continue;
    GLOBAL_students.insert(std::make_pair(tmp,std::map<std::string,LectureResult>()));
  }

  // ----------------------------------------------------------------------------
  // prepare a structure with the core poll information (name & correct responses)

  std::ifstream data_file("raw_data/polls/poll_questions.json");
  if (!data_file.good()) return;
  nlohmann::json j_data = nlohmann::json::parse(data_file);

  for (nlohmann::json::iterator itr = j_data.begin(); itr != j_data.end(); itr++) {
    int poll_id = itr->find("id")->get<int>();
    assert (poll_id >= 0);
    std::string poll_name = itr->find("name")->get<std::string>();
    std::string release_date = itr->find("release_date")->get<std::string>();
    std::string question_type = "single-response-multiple-correct";
    nlohmann::json::iterator itr2 = itr->find("question_type");
    if (itr2 != itr->end()) question_type = itr2->get<std::string>();
    std::string status = itr->find("status")->get<std::string>();
    assert (status == "ended" || status == "closed" || status == "open");

    std::string lecture;
    int which;
    group_polls_by_date(poll_id,release_date,lecture,which);
    GLOBAL_lectures[lecture].insert(std::make_pair(which,Poll(which,poll_name,release_date,question_type)));
    GLOBAL_poll_id_map[poll_id] = std::make_pair(lecture,which);

    const typename nlohmann::json &tmp = (*itr)["correct_responses"];
    for (nlohmann::json::const_iterator itr2 = tmp.begin(); itr2 != tmp.end(); itr2++) {
      int option_id = itr2->get<int>();
      std::map<int,Poll>::iterator itr3 = GLOBAL_lectures[lecture].find(which);
      assert (itr3 != GLOBAL_lectures[lecture].end());
      itr3->second.addCorrectOption(option_id);
    }
  }

  // ----------------------------------------------------------------------------
  // load and store the responses from each student for each poll

  std::ifstream responses_file("raw_data/polls/poll_responses.json");
  assert (responses_file.good());
  nlohmann::json j_responses = nlohmann::json::parse(responses_file);

  for (nlohmann::json::iterator itr = j_responses.begin(); itr != j_responses.end(); itr++) {
    int poll_id = itr->find("id")->get<int>();
    assert (poll_id >= 0);
    const typename nlohmann::json &tmp = (*itr)["responses"];
    std::pair<std::string,int> w = GLOBAL_poll_id_map[poll_id];

    for (nlohmann::json::const_iterator itr2 = tmp.begin(); itr2 != tmp.end(); itr2++) {
      std::string username = itr2.key();

      // student response will now always be an array
      std::vector<int> student_responses;
      if ((*itr2).is_array()) {
        student_responses = itr2->get<std::vector<int> >();
      } else {
        // backwards compatible -- previously only one response was allowed
        int response = itr2->get<int>();
        student_responses.push_back(response);
      }

      std::map<int,Poll>::iterator itr3 = GLOBAL_lectures[w.first].find(w.second);
      assert (itr3 != GLOBAL_lectures[w.first].end());
      const std::vector<int> &c_options = itr3->second.getCorrectOptions();
      const std::vector<int> &w_options = itr3->second.getWrongOptions();
      const std::string &question_type = itr3->second.getType();
      
      // count the number of correct & incorrect choices
      int incorrect_choices = 0;
      int correct_choices = 0;
      for (unsigned int i = 0; i < student_responses.size(); i++) {
        int r = student_responses[i];
        std::vector<int>::const_iterator c_itr = std::find(c_options.begin(),c_options.end(),r);
        if (c_itr != itr3->second.getCorrectOptions().end()) {
          correct_choices++;
        } else {
          //std::vector<int>::const_iterator w_itr = std::find(w_options.begin(),w_options.end(),r);
          //assert (w_itr != itr3->second.getWrongOptions().end());
          incorrect_choices++;
        }
      }

      // "grade" the response to this question, depending on the question type
      bool full_credit = false;
      if (question_type == "single-response-single-correct") {
        assert (c_options.size() == 1);
        assert (correct_choices+incorrect_choices <= 1);
        full_credit = (correct_choices == 1);
      } else if (question_type == "single-response-multiple-correct") {
        assert (c_options.size() >= 1);
        assert (correct_choices+incorrect_choices <= 1);
        full_credit = (correct_choices == 1);
      } else if (question_type == "single-response-survey") {
        assert (c_options.size() >= 1);
        assert (w_options.size() == 0);
        assert (correct_choices <= 1);
        //assert (incorrect_choices == 0);
        full_credit = (correct_choices == 1);
      } else if (question_type == "multiple-response-exact") {
        assert (c_options.size() >= 1);
        full_credit = (incorrect_choices == 0 && correct_choices == int(c_options.size()));
      } else if (question_type == "multiple-response-flexible") {
        assert (c_options.size() >= 1);
        full_credit = (incorrect_choices == 0 && correct_choices >= 1);
      } else if (question_type == "multiple-response-survey") {
        assert (c_options.size() >= 1);
        assert (w_options.size() == 0);
        //assert (incorrect_choices == 0);
        full_credit = (correct_choices >= 1);
      } else {
        std::cout << "OOPS: unknown question type '" << question_type << "'" << std::endl;
        assert (0);
      }

      if (full_credit) {
        itr3->second.incrCorrectClicks();
        GLOBAL_students[username][w.first].correct++;
      } else {
        itr3->second.incrWrongClicks();
        GLOBAL_students[username][w.first].wrong++;
      }
    }
  }

  //Update the correct and incorrect counts for each student
  for (unsigned int i = 0; i < students.size(); i++) {
    std::string tmp = students[i]->getUserName();
    std::string section = students[i]->getSection();
    if (section == "null") continue;
    for(std::map<std::string, LectureResult>::const_iterator it = GLOBAL_students[tmp].begin(); it != GLOBAL_students[tmp].end(); it++){
      students[i]->incrementPollsCorrect((it->second).correct);
      students[i]->incrementPollsIncorrect((it->second).wrong);
    }
  }
}

// =======================================================================================
// =======================================================================================
// Output an individual report for each student (which will be pasted
// into their individual rainbow grades report).
// 
void SavePollReports(const std::vector<Student*> &students) {

  // make a string representing today's date:  yyyy-mm-dd
  static std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  time_t tt = std::chrono::system_clock::to_time_t(now);
  tm local_tm = *localtime(&tt);
  std::stringstream ss;
  ss << local_tm.tm_year + 1900 << "-"
     << std::setw(2) << std::setfill('-') << local_tm.tm_mon + 1 << "-"
     << std::setw(2) << std::setfill('-') << local_tm.tm_mday;
  std::string today_string = ss.str();  

  std::cout << "TODAY " << today_string << std::endl;

  std::ofstream late_days_ostr("late_days.csv");
  if (GLOBAL_lectures.size() == 0) return;
  system ("mkdir -p student_poll_reports");
  
  for (std::map<std::string,std::map<std::string,LectureResult> >::iterator s = GLOBAL_students.begin();
       s != GLOBAL_students.end(); s++) {

    std::string username = s->first;
    std::ofstream student_ostr("student_poll_reports/"+username+".html");
    assert (student_ostr.good());
    
    student_ostr << "<h3> Lecture Participation Polls for: " << username << "</h3>" << std::endl;
    student_ostr << "<table cellpadding=5 style=\"border:1px solid #aaaaaa; background-color:#ffffff;\">" << std::endl;
    student_ostr << "<tr><td align=center>Lecture</td><td>#&nbsp;correct</td><td>#&nbsp;incorrect</td><td>#&nbsp;no&nbsp;response</td><td>Total Points</td><td>&nbsp;</td></tr>" << std::endl;
    int late_days = 1;
    
    float total = 0;

    int prev_total = 0;
    int which_lecture = 0;

    for (std::map<std::string,std::map<int,Poll> >::iterator it = GLOBAL_lectures.begin(); it != GLOBAL_lectures.end(); it++) {
      which_lecture++;
      int num = it->second.size();
      std::string lect = it->first;

      if (lect > today_string) continue;
      int correct = s->second[lect].correct;
      int wrong = s->second[lect].wrong;
      total += correct;
      total += 0.5 * wrong;
      Poll p = it->second.begin()->second;

      std::string foo = convert_date(p.getDate());
      
      std::string THING = "";
      if (earnedlatetoday(prev_total,total)) {
        late_days++;
        late_days_ostr << username << "," << foo << "," << late_days << "\r" << std::endl;
        THING = "EARNED LATE DAY #" + std::to_string(late_days) + " on " + foo;
      }

      Student *student_obj = NULL;
      for (unsigned int i = 0; i < students.size(); i++) {
        if (students[i]->getUserName() == username)
          student_obj = students[i];
      }
      assert (student_obj != NULL);
      
      if (student_obj->get_bonus_late_day(which_lecture)) {
        late_days++;
        late_days_ostr << username << "," << foo << "," << late_days << "\r" << std::endl;
        THING += " BONUS LATE DAY #" + std::to_string(late_days) + " on " + foo;
      }


      
      prev_total = total;
      student_ostr << "<tr><td align=center>" << lect << "</td><td align=center>"
                   << correct << "</td><td align=center>"
                   << wrong << "</td><td align=center>"
                   << num-correct-wrong << "</td><td align=center>"
                   << total << "</td><td>"
                   << THING << "</td></tr>" << std::endl;
    
    }

    student_ostr << "</table>" << std::endl;

    student_ostr << "<br><p>IMPORTANT:  Late days must be earned before the homework due date.<br>You may not use a late day earned on Friday on the homework due the night before.<br>You earn your second late day after the first 15 points, and an additional day for<br>every 30 points after that (45, 75, 105, 135)</p></table>" << std::endl;
    
  }
}

// =======================================================================================
