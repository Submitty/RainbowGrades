#ifndef PROCESS_GRADES__STRING_UTILITY_H
#define PROCESS_GRADES__STRING_UTILITY_H
#include <string>

inline std::string tolower(const std::string &s) {
  std::string answer;
  for (unsigned int i = 0; i < s.size(); i++) {
    answer += tolower(s[i]);
  }
  return answer;
}

inline std::string spacify(const std::string &s) {
  std::string tmp = "";
  for (unsigned int i = 0; i < s.size(); i++) {
    tmp += std::string(1,s[i]) + " ";
  }
  return tmp;
}

#endif // PROCESS_GRADES__STRING_UTILITY_H
