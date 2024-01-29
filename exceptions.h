#ifndef PROCESS_GRADES_EXCEPTIONS_H
#define PROCESS_GRADES_EXCEPTIONS_H
#include <stdexcept>

namespace rg {
class RainbowGradesException : public std::logic_error {
public:
  using std::logic_error::logic_error;
};
class InvalidGradeException : public RainbowGradesException {
public:
  using RainbowGradesException::RainbowGradesException;
};
class OutOfRange : public RainbowGradesException {
public:
  using RainbowGradesException::RainbowGradesException;
};
class InvalidID : public RainbowGradesException {
public:
  using RainbowGradesException::RainbowGradesException;
};
class InvalidArgument : public RainbowGradesException {
public:
  using RainbowGradesException::RainbowGradesException;
};

} // namespace rg

#endif // PROCESS_GRADES_EXCEPTIONS_H
