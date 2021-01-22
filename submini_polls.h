#include <vector>
#include <string>
#include "student.h"


class Poll {
public:
  Poll(int p=-1, std::string n="INVALID", std::string d="INVALID") : poll(p),name(n),release_date(d),correct_click(0),wrong_click(0) {}
  int poll;
  std::string name;
  std::string release_date;
  std::vector<int> correct_options;
  std::vector<int> wrong_options;
  bool is_correct(int option) {
    for (int i=0; i < correct_options.size(); i++) {
      if (option == correct_options[i])
        return true;
    }
    return false;
  }
  int correct_click;
  int wrong_click;
};


class LectureResult {
public:
  LectureResult() : correct(0),wrong(0),no_response(0) {}
  int correct;
  int wrong;
  int no_response;
};


void LoadPolls(const std::vector<Student*> &students);
void SavePollReports(const std::vector<Student*> &students);
