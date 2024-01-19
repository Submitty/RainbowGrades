#ifndef _GRADE_H_
#define _GRADE_H_

#include <string>

class Grade {
public:
  explicit Grade(const std::string &v);
private:
  friend bool operator< (const Grade &a, const Grade &b);
  std::string value;
};


bool operator< (const Grade &a, const Grade &b);

#endif
