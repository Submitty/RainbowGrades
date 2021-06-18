#include <vector>
#include <string>
#include "student.h"


class Poll {
public:
  // CONSTRUCTOR
  Poll(int p, std::string n, std::string d, std::string t)
    : poll_id(p),name(n),release_date(d),question_type(t),
      correct_click(0),wrong_click(0) {}

  // ACCESSORS
  int getID() const { return poll_id; }
  const std::string getName() const { return name; }
  const std::string getDate() const { return release_date; }
  const std::string getType() const { return question_type; }
  const std::vector<int>& getCorrectOptions() const { return correct_options; }
  const std::vector<int>& getWrongOptions() const { return wrong_options; }
  int getCorrectClicks() const { return correct_click; }
  int getWrongClicks() const { return wrong_click; }

  bool is_correct(int option) {
    for (int i=0; i < correct_options.size(); i++) {
      if (option == correct_options[i])
        return true;
    }
    return false;
  }

  // MODIFIERS
  void incrCorrectClicks() { correct_click++; }
  void incrWrongClicks() { wrong_click++; }
  void addCorrectOption(int x) { correct_options.push_back(x); }

private:

  // REPRESENTATION
  int poll_id;
  std::string name;
  std::string release_date;
  std::string question_type;
  std::vector<int> correct_options;
  std::vector<int> wrong_options;
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
