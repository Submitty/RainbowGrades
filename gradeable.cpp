#include <string>
#include <gradeable.h>
#include <iostream>

std::string gradeable_to_string(const GRADEABLE_ENUM &g) {
  switch (g) {
  case GRADEABLE_ENUM::HOMEWORK: return "HOMEWORK";
  case GRADEABLE_ENUM::ASSIGNMENT: return "ASSIGNMENT";
  case GRADEABLE_ENUM::PROBLEM_SET: return "PROBLEM_SET";
  case GRADEABLE_ENUM::QUIZ: return "QUIZ";
  case GRADEABLE_ENUM::TEST: return "TEST";
  case GRADEABLE_ENUM::EXAM: return "EXAM";
  case GRADEABLE_ENUM::EXERCISE: return "EXERCISE";
  case GRADEABLE_ENUM::LECTURE_EXERCISE: return "LECTURE_EXERCISE";
  case GRADEABLE_ENUM::READING: return "READING";
  case GRADEABLE_ENUM::WORKSHEET: return "WORKSHEET";
  case GRADEABLE_ENUM::LAB: return "LAB";
  case GRADEABLE_ENUM::RECITATION: return "RECITATION";
  case GRADEABLE_ENUM::PROJECT: return "PROJECT";
  case GRADEABLE_ENUM::PARTICIPATION: return "PARTICIPATION";
  case GRADEABLE_ENUM::NOTE: return "NOTE";
  case GRADEABLE_ENUM::NONE: return "NONE";
    // note we don't have a default case so that the compiler will warn if missing a case
  }
  std::cerr << "ERROR!  UNKNOWN GRADEABLE" << std::endl;
  exit(0);
}

bool string_to_gradeable_enum(const std::string &s, GRADEABLE_ENUM &return_value) {
  std::string s2;
  for (unsigned int i = 0; i < s.size(); ++i) {
    s2.push_back(std::tolower(s[i]));
  }
  std::replace( s2.begin(), s2.end(), '-', '_');
  if (s2 == "hw" || s2 == "homework")          { return_value = GRADEABLE_ENUM::HOMEWORK;          return true;  }
  if (s2 == "assignment")                      { return_value = GRADEABLE_ENUM::ASSIGNMENT;        return true;  }
  if (s2 == "problem_set")                     { return_value = GRADEABLE_ENUM::PROBLEM_SET;       return true;  }
  if (s2 == "quiz" || s2 == "quizze")          { return_value = GRADEABLE_ENUM::QUIZ;              return true;  }
  if (s2 == "test")                            { return_value = GRADEABLE_ENUM::TEST;              return true;  }
  if (s2 == "exam")                            { return_value = GRADEABLE_ENUM::EXAM;              return true;  }
  if (s2 == "exercise")                        { return_value = GRADEABLE_ENUM::EXERCISE;          return true;  }
  if (s2 == "lecture_exercise")                { return_value = GRADEABLE_ENUM::LECTURE_EXERCISE;  return true;  }
  if (s2 == "reading")                         { return_value = GRADEABLE_ENUM::READING;           return true;  }
  if (s2 == "worksheet")                       { return_value = GRADEABLE_ENUM::WORKSHEET;         return true;  }
  if (s2 == "lab")                             { return_value = GRADEABLE_ENUM::LAB;               return true;  }
  if (s2 == "recitation")                      { return_value = GRADEABLE_ENUM::RECITATION;        return true;  }
  if (s2 == "project")                         { return_value = GRADEABLE_ENUM::PROJECT;           return true;  }
  if (s2 == "participation")                   { return_value = GRADEABLE_ENUM::PARTICIPATION;     return true;  }
  if (s2 == "instructor_note" || s2 == "note") { return_value = GRADEABLE_ENUM::NOTE;              return true;  }
  if (s2 == "note")                            { return_value = GRADEABLE_ENUM::NOTE;              return true;  }
  if (s2.substr(0,4) == "none")                { return_value = GRADEABLE_ENUM::NOTE;              return true;  }
  return false;
}
