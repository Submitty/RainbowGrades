#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <ctime>
#include <cmath>

#include "student.h"
#include "grade.h"
#include "table.h"
#include "benchmark.h"

#include <nlohmann/json.hpp>

#define grey_divider "aaaaaa"

#include "constants_and_globals.h"

extern std::string OUTPUT_FILE;
extern std::string OUTPUT_CSV_FILE;
extern std::string RG_VERSION_FILE;

extern std::string ALL_STUDENTS_OUTPUT_DIRECTORY;
extern std::string ALL_STUDENTS_OUTPUT_DIRECTORY_CSV;

extern Student* AVERAGE_STUDENT_POINTER;
extern Student* STDDEV_STUDENT_POINTER;

extern std::string GLOBAL_sort_order;

extern int GLOBAL_ACTIVE_TEST_ZONE;

// ==========================================================

std::string HEX(int h) {
  std::stringstream ss;
  ss << std::hex << std::setw(2) << std::setfill('0') << h;
  return ss.str();
}

int UNHEX(std::string s) {
  assert (s.size() == 2);
  int h;
  std::stringstream ss(s);
  ss >> std::hex >> h;
  assert (h >= 0 && h <= 255);
  return h;
}


// colors for grades
const std::string GradeColor(const std::string &grade) {
  if      (grade == "A" ) return HEX(200)+HEX(200)+HEX(255); 
  else if (grade == "A-") return HEX(200)+HEX(235)+HEX(255); 
  else if (grade == "B+") return HEX(219)+HEX(255)+HEX(200); 
  else if (grade == "B" ) return HEX(237)+HEX(255)+HEX(200); 
  else if (grade == "B-") return HEX(255)+HEX(255)+HEX(200); 
  else if (grade == "C+") return HEX(255)+HEX(237)+HEX(200); 
  else if (grade == "C" ) return HEX(255)+HEX(219)+HEX(200); 
  else if (grade == "C-") return HEX(255)+HEX(200)+HEX(200); 
  else if (grade == "D+") return HEX(255)+HEX(100)+HEX(100); 
  else if (grade == "D" ) return HEX(255)+HEX(  0)+HEX(  0); 
  else if (grade == "F" ) return HEX(200)+HEX(  0)+HEX(  0); 
  else return "ffffff";
}

// ==========================================================

float compute_average(const std::vector<float> &vals) {
  if (vals.size() == 0) return 0;
  assert (vals.size() > 0);
  float total = 0;
  for (std::size_t i = 0; i < vals.size(); i++) {
    total += vals[i];
  }
  return total / float (vals.size());
}


float compute_stddev(const std::vector<float> &vals, float average) {
  if (vals.size() == 0) return 0;
  assert (vals.size() > 0);
  float total = 0;
  for (std::size_t i = 0; i < vals.size(); i++) {
    total += (vals[i]-average)*(vals[i]-average);
  }
  return sqrt(total / float (vals.size()) );
}

// ==========================================================

int convertYear(const std::string &major) {
  if (major == "FR") return 1;
  if (major == "SO") return 2;
  if (major == "JR") return 3;
  if (major == "SR") return 4;
  if (major == "FY") return 5;
  if (major == "GR") return 6;
  else return 10;
}

int convertMajor(const std::string &major) {
  if (major == "CSCI") return 20;
  if (major == "ITWS" || major == "ITEC") return 19;
  if (major == "CSYS") return 18;
  if (major == "GSAS") return 17;
  if (major == "MATH") return 16;
  if (major == "COGS" || major == "PSYC") return 15;
  if (major == "ELEC") return 14;
  if (major == "PHYS" || major == "APHY") return 13;
  if (major == "BMGT" || 
      major == "ISCI" || 
      major == "ENGR" || 
      major == "USCI" ||
      major == "DSIS" ||
      major == "ECON" ||
      major == "EART" ||
      major == "CHEG" || 
      major == "MECL" ||
      major == "MGTE" ||
      major == "UNGS" ||
      major == "BMED" ||
      major == "MECL" ||
      major == "BFMB" ||
      major == "ARCH" ||
      major == "FERA" ||
      major == "CHEM" ||
      major == "MGMT" ||
      major == "NUCL" ||
      major == "MATL" ||
      major == "") return 0;
  else return 10;
}

// ==========================================================

class Color {
public:
  Color(int r_=0, int g_=0, int b_=0) : r(r_),g(g_),b(b_) {}
  Color(const std::string& s) {
    r = UNHEX(s.substr(0,2));
    g = UNHEX(s.substr(2,2));
    b = UNHEX(s.substr(4,2));
  }
  int r,g,b;
};

std::string coloritcolor(float val,
                         float perfect,
                         float a,
                         float b,
                         float c,
                         float d) {

  //check for nan
  if (val != val) return "ffffff";
  if (std::isinf(val)) return "00ff00";
  if (std::isnan(perfect)) return "00ff00";

  //std::cout << "coloritcolor " << val << " " << perfect << " " << a << " " << b << " " << c << " " << d << std::endl;
  assert (perfect >= a &&
          a >= b &&
          b >= c &&
          c >= d &&
          d >= 0);

  if (val < 0.00001) return "ffffff";
  else if (val > perfect) return GetBenchmarkColor("extracredit");
  else {
    float alpha;
    Color c1,c2;

    static Color perfect_color(GetBenchmarkColor("perfect"));
    static Color a_color(GetBenchmarkColor("lowest_a-"));
    static Color b_color(GetBenchmarkColor("lowest_b-"));
    static Color c_color(GetBenchmarkColor("lowest_c-"));
    static Color d_color(GetBenchmarkColor("lowest_d"));

    if (val >= a) {
      if (fabs(perfect-a) < 0.0001) alpha = 0;
      else alpha = (perfect-val)/float(perfect-a);
      c1 = perfect_color;
      c2 = a_color;
    }
    else if (val >= b) {
      if (fabs(a-b) < 0.0001) alpha = 0;
      else alpha = (a-val)/float(a-b);
      c1 = a_color;
      c2 = b_color;
    }
    else if (val >= c) {
      if (fabs(b-c) < 0.0001) alpha = 0;
      else alpha = (b-val)/float(b-c);
      c1 = b_color;
      c2 = c_color;
    }
    else if (val >= d) {
      if (fabs(c-d) < 0.0001) alpha = 0;
      else alpha = (c-val)/float(c-d);
      c1 = c_color;
      c2 = d_color;
    }
    else {
      return GetBenchmarkColor("failing");
    }

    float red   = (1-alpha) * c1.r + (alpha) * c2.r;
    float green = (1-alpha) * c1.g + (alpha) * c2.g;
    float blue  = (1-alpha) * c1.b + (alpha) * c2.b;

    return HEX(red) + HEX(green) + HEX(blue);

  }
}

void coloritcolor(std::ostream &ostr,
                  float val,
                  float perfect,
                  float a,
                  float b,
                  float c,
                  float d) {
  ostr << coloritcolor(val,perfect,a,b,c,d);
}

void colorit_year(std::ostream &ostr, const std::string& s) {
  if (s == "FR") {
    ostr << "<td align=center bgcolor=ffffff>" << s << "</td>";
  } else if (s == "SO") {
    ostr << "<td align=center bgcolor=dddddd>" << s << "</td>";
  } else if (s == "JR") {
    ostr << "<td align=center bgcolor=bbbbbb>" << s << "</td>";
  } else if (s == "SR") {
    ostr << "<td align=center bgcolor=999999>" << s << "</td>";
  } else if (s == "GR") {
    ostr << "<td align=center bgcolor=777777>" << s << "</td>";
  } else if (s == "FY") {
    ostr << "<td align=center bgcolor=555555>" << s << "</td>";
  } else if (s == "") {
    ostr << "<td align=center bgcolor=ffffff>&nbsp;</td>";
  } else {
    std::cout << "EXIT " << s << std::endl;
    exit(0);
  }
}



void colorit_major(std::ostream &ostr, const std::string& s) {
  int m = convertMajor(s);
  ostr << "<td align=center bgcolor=";
  coloritcolor(ostr,m,19.5,17,15,11,10);
  ostr << ">" << s << "</td>";
}
   


void colorit_section(std::ostream &ostr,
                     std::string section, bool for_instructor, const std::string &color) {

  std::string section_name;

  if (validSection(section)) 
    section_name = sectionNames[section];
  std::string section_color = sectionColors[section_name];

  if (section == "null") {
    section_color=color;
  }

  if (for_instructor) {
    if (section != "null") {
      ostr << "<td align=center bgcolor=" << section_color << ">" << section << "&nbsp;(" << section_name << ")</td>";
    } else {
      ostr << "<td align=center bgcolor=" << section_color << ">&nbsp;</td>" << std::endl;
    }
  } else {
    if (section != "null") {
      ostr << "<td align=center>" << section << "</td>";
    } else {
      ostr << "<td align=center bgcolor=" << section_color << ">&nbsp;</td>" << std::endl;
    }
  }

}

void colorit_section2(std::string section, std::string &color, std::string &label) {
  std::string section_name;
  if (validSection(section)) {
    section_name = sectionNames[section];
    color = sectionColors[section_name];
    std::stringstream ss;
    ss << section << "&nbsp;(" << sectionNames[section] << ")";
    label = ss.str();
  }
}



void colorit(std::ostream &ostr,
             float val,
             float perfect,
             float a,
             float b,
             float c,
             float d,
             int precision=1,
             bool centered=false,
             std::string bonus_text="") {
  if (centered)
    ostr << "<td align=center bgcolor=";
  else
    ostr << "<td align=right bgcolor=";
  coloritcolor(ostr,val,perfect,a,b,c,d);
  ostr << ">";
  if (val < 0.0000001) {
    ostr << "&nbsp;";
  } else if (precision == 1) {  
    ostr << std::dec << val << "&nbsp;" << bonus_text;
  } else {
    assert (precision == 0);
    ostr << std::dec << (int)val;
  }
  ostr << "</td>"; 
}

// ==========================================================

//void PrintExamRoomAndZoneTable(std::ofstream &ostr, Student *s, const nlohmann::json &special_message) {
void PrintExamRoomAndZoneTable(const std::string &g_id, nlohmann::json &mj, Student *s, const nlohmann::json &special_message) {

  Student *s_tmp = s;

  if (special_message.size() > 0) {
    
    /*ostr << "<table border=1 cellpadding=5 cellspacing=0 style=\"background-color:#ddffdd; width:auto;\">\n";
    ostr << "<tr><td>\n";
    ostr << "<table border=0 cellpadding=5 cellspacing=0>\n";*/

    assert (special_message.find("title") != special_message.end());
    std::string title = special_message.value("title","MISSING TITLE");

    assert (special_message.find("description") != special_message.end());
    std::string description = special_message.value("description","provided_files.zip");

    //ostr << "<h3>" << title << "</h3>" << std::endl;
    mj["special_message"]["title"] = title;

    assert (special_message.find("files") != special_message.end());
    nlohmann::json files = *(special_message.find("files"));
    int num_files = files.size();
    assert (num_files >= 1);

    std::string username = s->getUserName();
    int A = 54059; /* a prime */
    int B = 76963; /* another prime */
    int FIRSTH = 37; /* also prime */

    unsigned int tmp = FIRSTH;
    for (std::size_t i = 0; i < username.size(); i++) {
      tmp = (tmp * A) ^ (username[i] * B);
      s++;
    }

    int which = (tmp % num_files)+1;
    std::string filename = files.value(std::to_string(which),"");
    assert (filename != "");

    mj["special_message"]["filename"] = filename;
    mj["special_message"]["description"] = description;
    /*ostr << "  <tr><td><a href=\"" << filename << "\" download=\"provided_files.zip\">" << description << "</a></td></tr>\n";
    ostr << "</table>\n";
    ostr << "</tr></td>\n";
    ostr << "</table>\n";*/

    s = s_tmp; //Reset the student pointer in case exam seating needs it.
  }

  // ==============================================================

  if ( DISPLAY_EXAM_SEATING == false) return;

  std::string room = GLOBAL_EXAM_DEFAULT_ROOM;
  std::string building = GLOBAL_EXAM_DEFAULT_BUILDING;
  std::string zone = "SEE INSTRUCTOR";
  std::string time = GLOBAL_EXAM_TIME;
  std::string row = "";
  std::string seat = "";
  if (s->getSection() == "null") {
    //room = "";
    //zone = "";
    time = "";
  }
  if (s->getExamRoom() == "") {
    //std::cout << "NO ROOM FOR " << s->getUserName() << std::endl;
  } else {
    room = s->getExamRoom();
    building = s->getExamBuilding();
    zone = s->getExamZone();
    row = s->getExamRow();
    seat = s->getExamSeat();
    if (s->getExamTime() != "") {
      time = s->getExamTime();
    }
  }
  if (zone == "SEE_INSTRUCTOR") {
    zone = "SEE INSTRUCTOR";
  }


  /*ostr << "<table style=\"border:1px solid yellowgreen; background-color:#ddffdd; width:auto;\" >\n";
  //  ostr << "<table border=\"1\" cellpadding=5 cellspacing=0 style=\"border:1px solid yellowgreen; background-color:#ddffdd;\">\n";
  ostr << "<tr><td>\n";
  ostr << "<table border=0 cellpadding=5 cellspacing=0>\n";
  ostr << "  <tr><td colspan=2>" << GLOBAL_EXAM_TITLE << "</td></tr>\n";
  ostr << "  <tr><td>" << GLOBAL_EXAM_DATE << "</td><td align=center>" << time << "</td></tr>\n";
  ostr << "  <tr><td>Your room assignment: </td><td align=center>" << room << "</td></tr>\n";
  ostr << "  <tr><td>Your zone assignment: </td><td align=center>" << zone << "</td></tr>\n";
  if (row != "N/A" && row !="") {
    ostr << "  <tr><td>Your row assignment: </td><td align=center>" << row << "</td></tr>\n";
  }
  if (seat != "N/A" && seat !="") {
    ostr << "  <tr><td>Your seat assignment: </td><td align=center>" << seat << "</td></tr>\n";
  }
  ostr << "</table>\n";
  ostr << "</tr></td>\n";

  if (s->getExamZoneImage() != "") {
    ostr << "<tr><td style=\"background-color:#ffffff;\"><img src=\"zone_images/" + s->getExamZoneImage() + "\"></td></tr>\n";
  }


  // ostr << "</table>\n";*/

  mj["title"] = GLOBAL_EXAM_TITLE;
  mj["date"] = GLOBAL_EXAM_DATE;
  mj["room"] = room;
  mj["building"] = building;
  mj["time"] = time;
  mj["gradeable"] = g_id;
  mj["username"] = s->getUserName();
  mj["zone"] = zone;
  if (row != "N/A" && row !="") {
    mj["row"] = row;
  }
  if (seat != "N/A" && seat !="") {
    mj["seat"] = seat;
  }

  // It shouldn't be Rainbow Grades job to know that on the server it's zone_images/
  // this should be done server side. Also allows server to let course specify hosting
  // images on server or at a different location.
  if (s->getExamZoneImage() != "") {
    mj["title"] = s->getExamZoneImage();
  }

  // might change format...
  mj["seating"] = mj;

  std::string x1 = s->getExamZone();
  std::string x2 = s->getZone(GLOBAL_ACTIVE_TEST_ZONE);

  if (x2.size() > 0) {
    assert (x1.size() > 0);
    if (x1.size() > 1 || std::toupper(x1[0]) != std::toupper(x2[0]) || x2.find('?') != std::string::npos || x2.size() == 1) {
      std::cout << "WRONG ZONE" << s->getUserName() << " " << x1 << " " <<x2  << std::endl;
    }
  }
}


// ====================================================================================================
// ====================================================================================================
// ====================================================================================================

void end_table(std::ofstream &ostr,  bool for_instructor, Student *s);

/*
void start_table_open_file(bool for_instructor,
                 const std::vector<Student*> &students, int rank, int month, int day, int year,
                 enum GRADEABLE_ENUM which_gradeable_enum) {

  / *
  ostr.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
  try {
    ostr.open(filename.c_str());
  }
  catch (std::ofstream::failure e) {
    std::cout << "FAILED TO OPEN " << filename << std::endl;
    std::cerr << "Exception opening/reading file";
    exit(0);
  }
  * /
}
*/

void SelectBenchmarks(std::vector<int> &select_students, const std::vector<Student*> &students,
                      Student *sp, Student *sa, Student *sb, Student *sc, Student *sd) {
  int myrow = 1;

  int offset = select_students.size();
  select_students.resize(select_students.size()+NumVisibleBenchmarks());

  for (unsigned int stu= 0; stu < students.size(); stu++) {
    std::string default_color="ffffff";
    Student *this_student = students[stu];
    myrow++;

    int which;

    if (this_student->getLastName() == "") {
      if (this_student == sp) {
        which = WhichVisibleBenchmark("perfect");
        if (which >= 0) select_students[offset+which]=myrow;
      } else if (this_student == AVERAGE_STUDENT_POINTER) {
        which = WhichVisibleBenchmark("average");
        if (which >= 0) select_students[offset+which]=myrow;
      } else if (this_student == STDDEV_STUDENT_POINTER) {
        which = WhichVisibleBenchmark("stddev");
        if (which >= 0) select_students[offset+which]=myrow;
      } else if (this_student == sa) {
        which = WhichVisibleBenchmark("lowest_a-");
        if (which >= 0) select_students[offset+which]=myrow;
      } else if (this_student == sb) {
        which = WhichVisibleBenchmark("lowest_b-");
        if (which >= 0) select_students[offset+which]=myrow;
      } else if (this_student == sc) {
        which = WhichVisibleBenchmark("lowest_c-");
        if (which >= 0) select_students[offset+which]=myrow;
      } else if (this_student == sd) {
        which = WhichVisibleBenchmark("lowest_d");
        if (which >= 0) select_students[offset+which]=myrow;
      }
    }
  }
}


std::ofstream GLOBAL_EWS_OUTPUT;

std::ofstream GLOBAL_GRADES_OUTPUT;

void start_table_output( bool /*for_instructor*/,
                         const std::vector<Student*> &students, int rank, int month, int day, int year,
                         Student *sp, Student *sa, Student *sb, Student *sc, Student *sd, bool csv_mode) {

  std::vector<int> all_students;
  std::vector<int> select_students;
  std::vector<int> instructor_data;
  std::vector<int> student_data;

  Table table;

  GLOBAL_EWS_OUTPUT = std::ofstream("ews_output.txt");
  assert (GLOBAL_EWS_OUTPUT.good());

  GLOBAL_GRADES_OUTPUT = std::ofstream("grades_output.txt");
  assert (GLOBAL_GRADES_OUTPUT.good());

  // =====================================================================================================
  // =====================================================================================================
  // DEFINE HEADER ROW
  int counter = 0;
  table.set(0,counter++,TableCell("ffffff","#"));
  table.set(0,counter++,TableCell("ffffff","SECTION"));
  table.set(0,counter++,TableCell("ffffff","reg type"));
  if (DISPLAY_INSTRUCTOR_NOTES) {
    table.set(0,counter++,TableCell("ffffff","part."));
    table.set(0,counter++,TableCell("ffffff","under."));
    table.set(0,counter++,TableCell("ffffff","notes"));
  }
  student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","USERNAME"));
  student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","NUMERIC ID"));
  if (DISPLAY_INSTRUCTOR_NOTES || DISPLAY_FINAL_GRADE) {
    table.set(0,counter++,TableCell("ffffff","FAMILY (LEGAL)"));
    table.set(0,counter++,TableCell("ffffff","GIVEN (LEGAL)"));
  }
  int last_name_counter=counter;
  table.set(0,counter++,TableCell("ffffff","FAMILY"));
  student_data.push_back(counter);  table.set(0,counter++,TableCell("ffffff","GIVEN"));
  student_data.push_back(last_name_counter);
  student_data.push_back(counter);  table.set(0,counter++,TableCell(grey_divider));

  if (DISPLAY_EXAM_SEATING) {
    student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","exam room"));
    student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","exam zone"));
    student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","exam row"));
    student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","exam seat"));
    student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","exam time"));
    student_data.push_back(counter); table.set(0,counter++,TableCell(grey_divider));
  }

  if (DISPLAY_GRADE_SUMMARY) {
    student_data.push_back(counter);  table.set(0,counter++,TableCell("ffffff","OVERALL"));
  }
  if(DISPLAY_RANK_TO_INDIVIDUAL) {
    student_data.push_back(counter);  table.set(0, counter++, TableCell("ffffff", "RANK"));
  }
  if (DISPLAY_GRADE_SUMMARY) {
    student_data.push_back(counter);  table.set(0,counter++,TableCell(grey_divider));
  }

  if (DISPLAY_FINAL_GRADE) {
    std::cout << "DISPLAY FINAL GRADE" << std::endl;
    student_data.push_back(counter); table.set(0,counter++,TableCell("ffffff","FINAL GRADE"));
    student_data.push_back(counter); table.set(0,counter++,TableCell(grey_divider));
    if (DISPLAY_ACADEMIC_SANCTION_DETAILS) {
      table.set(0,counter++,TableCell("ffffff","RAW GRADE"));
      table.set(0,counter++,TableCell(grey_divider));
    }
  } 

  if (DISPLAY_GRADE_SUMMARY) {
    // ----------------------------
    // % OF OVERALL AVERAGE FOR EACH GRADEABLE
    for (unsigned int i = 0; i < ALL_GRADEABLES.size(); i++) {
      enum GRADEABLE_ENUM g = ALL_GRADEABLES[i];
      if (g == GRADEABLE_ENUM::NOTE) {
        assert (GRADEABLES[g].getPercent() < 0.01);
        continue;
      }
      student_data.push_back(counter);  table.set(0,counter++,TableCell("ffffff",gradeable_to_string(g)+" %"));
    }
    student_data.push_back(counter);  table.set(0,counter++,TableCell(grey_divider));
  }

  // ----------------------------
  // DETAILS OF EACH GRADEABLE
  if (DISPLAY_GRADE_DETAILS) {
    for (unsigned int i = 0; i < ALL_GRADEABLES.size(); i++) {
      GRADEABLE_ENUM g = ALL_GRADEABLES[i];
      for (int j = 0; j < GRADEABLES[g].getCount(); j++) {
        if (g != GRADEABLE_ENUM::NOTE) {
          student_data.push_back(counter);
        }
        std::string gradeable_id = GRADEABLES[g].getID(j);
        std::string gradeable_name = "";
        if (GRADEABLES[g].hasCorrespondence(gradeable_id)) {
          gradeable_name = GRADEABLES[g].getCorrespondence(gradeable_id).second;
          //gradeable_name = spacify(gradeable_name);
        }
        if (gradeable_name == "")
          gradeable_name = "<em><font color=\"aaaaaa\">future "
            + tolower(gradeable_to_string(g)) + "</font></em>";
        table.set(0,counter++,TableCell("ffffff",gradeable_name));
      }
      if (g != GRADEABLE_ENUM::NOTE) {
        student_data.push_back(counter);
      }
      table.set(0,counter++,TableCell(grey_divider));

      if (g == GRADEABLE_ENUM::TEST && TEST_IMPROVEMENT_AVERAGING_ADJUSTMENT) {
        for (int j = 0; j < GRADEABLES[g].getCount(); j++) {
          student_data.push_back(counter);
          std::string gradeable_id = GRADEABLES[g].getID(j);
          std::string gradeable_name = "";
          if (GRADEABLES[g].hasCorrespondence(gradeable_id)) {
            gradeable_name = "Adjusted " + GRADEABLES[g].getCorrespondence(gradeable_id).second;
          }
          table.set(0,counter++,TableCell("ffffff",gradeable_name));
        }
        student_data.push_back(counter);  table.set(0,counter++,TableCell(grey_divider));
      }
    }

    if (DISPLAY_LATE_DAYS) {
      //Polls headers
      //TODO: This should be a separate setting/if-block if we allow courses to use polls without adding late days
      table.set(0,counter++,TableCell("ffffff","TOTAL POLL POINTS"));
      table.set(0,counter++,TableCell("ffffff","TOTAL POLLS ANSWERED"));
      table.set(0,counter++,TableCell("ffffff","CORRECT POLLS"));
      table.set(0,counter++,TableCell("ffffff","INCORRECT POLLS"));

      //Late days headers
      //student_data.push_back(counter);  table.set(0,counter++,TableCell("ffffff","ALLOWED LATE DAYS"));
      //student_data.push_back(counter);  table.set(0,counter++,TableCell("ffffff","USED LATE DAYS"));
      //student_data.push_back(counter);  table.set(0,counter++,TableCell("ffffff","EXCUSED EXTENSIONS"));
      student_data.push_back(counter);  table.set(0,counter++,TableCell(grey_divider));
    }
  }


  // =====================================================================================================
  // =====================================================================================================
  // HORIZONTAL GRAY DIVIDER
  for (int i = 0; i < table.numCols(); i++) {
    table.set(1,i,TableCell(grey_divider));
  }


  // header row
  select_students.push_back(0);
  select_students.push_back(1);
  select_students.push_back(-1);  // replace this with the real student!
  select_students.push_back(1);


  std::map<int,std::string> student_correspondences;

  // =====================================================================================================
  // =====================================================================================================
  // ALL OF THE STUDENTS

  SelectBenchmarks(select_students,students,sp,sa,sb,sc,sd);


  int myposition = 1;
  int myrow = 1;
  std::string last_section = "";
  for (unsigned int stu= 0; stu < students.size(); stu++) {

    Student *this_student = students[stu];
    
    std::string default_color="ffffff";

    myrow++;
    counter = 0;
    if (this_student->getLastName() == "") {
      if (this_student == sp) {
        default_color= coloritcolor(5,5,4,3,2,1);
      } else if (this_student == sa) {
        default_color= coloritcolor(4,5,4,3,2,1);
      } else if (this_student == sb) {
        default_color= coloritcolor(3,5,4,3,2,1);
      } else if (this_student == sc) {
        default_color= coloritcolor(2,5,4,3,2,1);
      } else if (this_student == sd) {
        default_color= coloritcolor(1,5,4,3,2,1);
      } 
      assert (default_color.size()==6);
      table.set(myrow,counter++,TableCell(default_color,""));
    } else {
      //std::cout << " WHO? " << this_student->getUserName() << std::endl;
      student_correspondences[myrow] = this_student->getUserName();
      if (GLOBAL_sort_order == "by_section" && this_student->getSection() != last_section) {
        myposition=1;
        last_section = this_student->getSection();
      }
      if (validSection(this_student->getSection())) {
        assert (default_color.size()==6);
        table.set(myrow,counter++,TableCell(default_color,std::to_string(myposition)));
        myposition++;
      } else {
        assert (default_color.size()==6);
        table.set(myrow,counter++,TableCell(default_color,""));
      }
    }

    
    std::string section_color = default_color;
    std::string section_label = "";
    std::string status = "";
    if(!csv_mode) {
        colorit_section2(this_student->getSection(), section_color, section_label);
        if (validSection(this_student->getSection())) {
          status = this_student->getRegistrationStatus();
          if (status == "withdrawn") section_color = default_color;
        }
    }
    else{
        if (validSection(this_student->getSection())) {
            section_label = sectionNames[this_student->getSection()];
            section_color = sectionColors[section_label];
            std::stringstream ss;
            section_label = this_student->getSection() + " (" + section_label + ")";
        }
    }
    if (status != "graded") section_color = default_color;
    assert (section_color.size()==6);
    table.set(myrow,counter++,TableCell(section_color,section_label));
    table.set(myrow,counter++,TableCell(default_color,status));

    if (DISPLAY_INSTRUCTOR_NOTES) {
      float participation = this_student->getParticipation();
      std::string color = coloritcolor(participation,5,4,3,2,1);
      table.set(myrow,counter++,TableCell(color,participation,1));
      float understanding = this_student->getUnderstanding();
      color = coloritcolor(understanding,5,4,3,2,1);
      table.set(myrow,counter++,TableCell(color,understanding,1));
      std::string notes;
      std::vector<std::string> ews = this_student->getEarlyWarnings();
      for (std::size_t i = 0; i < ews.size(); i++) {
        notes += ews[i];

        //std::cout << "FOUND EWS " << this_student->getUserName() << ews[i] << std::endl;

      }
      std::string other_note = this_student->getOtherNote();

      // TEMPORARY placeholder code until course section is added
      std::string crn = "99999";
      if (this_student->getSection() == "1") crn = "55037";
      if (this_student->getSection() == "2") crn = "57469";
      if (this_student->getSection() == "3") crn = "55038";
      if (this_student->getSection() == "4") crn = "57234";
      if (this_student->getSection() == "5") crn = "56274";
      if (this_student->getSection() == "6") crn = "55039";
      if (this_student->getSection() == "7") crn = "55328";
      if (this_student->getSection() == "8") crn = "57673";
      if (this_student->getSection() == "9") crn = "57871";
      if (this_student->getSection() == "10")crn = "58089";
        
      if (other_note != "" && other_note.find("EWS2") != std::string::npos) {
        
        std::string category = "FAILING";
        if (other_note.find("TEST") != std::string::npos) { category="TEST_PERFORMANCE"; }
        if (other_note.find("HW") != std::string::npos) { category="MISSING_INCOMPLETE_HW"; }
        if (other_note.find("ATTENDANCE") != std::string::npos) { category="ATTENDANCE"; }
        
        GLOBAL_EWS_OUTPUT << this_student->getSection() << ","
                          << crn << "," << this_student->getUserName() << ","
                          << this_student->getNumericID() << ","
                          << category << "," << other_note << std::endl;
        
      }

      std::string student_grade = this_student->grade(false,sd);      
      
      if (this_student->getSection() != "null" &&
          this_student->getSection() != "STAFF" &&
          this_student->getSection() != "ALAC"
          ) {
      
        GLOBAL_GRADES_OUTPUT << this_student->getSection() << ","
                             << crn << "," << this_student->getUserName() << ","
                             << this_student->getNumericID() << ","
                             << student_grade << std::endl;
      }
      
      std::string recommendation = this_student->getRecommendation();
      std::string THING;
      if(!csv_mode) {
          THING =
                  "<font color=\"ff0000\">" + notes + "</font> " +
                  "<font color=\"0000ff\">" + other_note + "</font> " +
                  "<font color=\"00bb00\">" + recommendation + "</font>";
      }
      else{
          THING = notes + " " + other_note + " " + recommendation;
        
      }
      assert (default_color.size()==6);
      table.set(myrow,counter++,TableCell(default_color,THING));
    }

    //counter+=3;
    assert (default_color.size()==6);
    table.set(myrow,counter++,TableCell(default_color,this_student->getUserName()));
    table.set(myrow,counter++,TableCell(default_color,this_student->getNumericID()));
    if (DISPLAY_INSTRUCTOR_NOTES || DISPLAY_FINAL_GRADE) {
      table.set(myrow,counter++,TableCell(default_color,this_student->getLastName()));
      table.set(myrow,counter++,TableCell(default_color,this_student->getFirstName()));
    }
    table.set(myrow,counter++,TableCell(default_color,this_student->getPreferredLastName()));
    table.set(myrow,counter++,TableCell(default_color,this_student->getPreferredFirstName()));
    table.set(myrow,counter++,TableCell(grey_divider));


    if (DISPLAY_EXAM_SEATING) {

      std::string room = GLOBAL_EXAM_DEFAULT_ROOM;
      std::string building = GLOBAL_EXAM_DEFAULT_BUILDING;
      std::string zone = "SEE INSTRUCTOR";
      std::string row = "";
      std::string seat = "";
      std::string time = GLOBAL_EXAM_TIME;

      if (this_student->getSection() == "null") { //LastName() == "") {
        room = "";
        building = "";
        zone = "";
        time = "";
      }
      if (this_student->getExamRoom() == "") {
        //std::cout << "NO ROOM FOR " << this_student->getUserName() << std::endl;
      } else {
        room = this_student->getExamRoom();
        building = this_student->getExamBuilding();
        zone = this_student->getExamZone();
        row = this_student->getExamRow();
        seat = this_student->getExamSeat();
        if (this_student->getExamTime() != "") {
          time = this_student->getExamTime();
        }
      }
      if (zone == "SEE_INSTRUCTOR") {
        zone = "SEE INSTRUCTOR";
      }

      table.set(myrow,counter++,TableCell("ffffff",building+" "+room));
      table.set(myrow,counter++,TableCell("ffffff",zone));
      table.set(myrow,counter++,TableCell("ffffff",row));
      table.set(myrow,counter++,TableCell("ffffff",seat));
      table.set(myrow,counter++,TableCell("ffffff",time));
      table.set(myrow,counter++,TableCell(grey_divider));
    }


    float grade;
    if (this_student->getUserName() == "AVERAGE" ||
        this_student->getUserName() == "STDDEV") {
      // Special case for overall average and standard deviation.
      // Mathematically, we can't simply add the std dev for the
      // different gradeables.  Note also: the average isn't a simple
      // addition either, since blank scores for a specific gradeable
      // are omitted from the average.
      std::vector<float> vals;
      for (unsigned int S = 0; S < students.size(); S++) {
        if (validSection(students[S]->getSection()) &&
            (!OmitSectionFromStats(students[S]->getSection()))) {
          vals.push_back(students[S]->overall());
        }
      }
      float tmp_average = compute_average(vals);
      if (this_student->getUserName() == "AVERAGE") {
        grade = tmp_average;
      } else {
        float tmp_std_dev = compute_stddev(vals,tmp_average);
        grade = tmp_std_dev;
      }
    } else if (this_student->getUserName() == "LOWEST D") {
      // don't show the overall average for the D/F border
      grade = 0;
    } else {
      grade = this_student->overall();
    }

    std::string color = coloritcolor(grade,
                                     sp->overall(),
                                     sa->overall(),
                                     sb->overall(),
                                     sc->overall(),
                                     sd->overall());
    if (this_student == STDDEV_STUDENT_POINTER) color="ffffff";
    assert (color.size()==6);
    if (DISPLAY_GRADE_SUMMARY) {
        table.set(myrow,counter++,TableCell(color,grade,2));
    }
    if(DISPLAY_RANK_TO_INDIVIDUAL) {
      if(validSection(this_student->getSection())) {
        table.set(myrow, counter++, TableCell(color, this_student->getRank(), "", 0, CELL_CONTENTS_VISIBLE, "right"));
      }
      else{
        table.set(myrow, counter++, TableCell("ffffff", "", "", 0, CELL_CONTENTS_VISIBLE, "right"));
      }
    }
    if (DISPLAY_GRADE_SUMMARY) {
      table.set(myrow,counter++,TableCell(grey_divider));
    }

    if (DISPLAY_FINAL_GRADE) {
      std::string g = this_student->grade(false,sd);      
      color = GradeColor(g);
      if (this_student->getAcademicSanctionPenalty() < -0.00000001) {
        g += "@";
      }
      assert (color.size()==6);
      table.set(myrow,counter++,TableCell(color,g,"",0,CELL_CONTENTS_VISIBLE,"center"));
      table.set(myrow,counter++,TableCell(grey_divider));

      if (DISPLAY_ACADEMIC_SANCTION_DETAILS) {
        std::string g2 = this_student->grade(true,sd);
        color = GradeColor(g2);
        table.set(myrow,counter++,TableCell(color,g2,"",0,CELL_CONTENTS_VISIBLE,"center"));
        table.set(myrow,counter++,TableCell(grey_divider));
      }
    }

    if (DISPLAY_GRADE_SUMMARY) {
    // ----------------------------
    // % OF OVERALL AVERAGE FOR EACH GRADEABLE
    for (unsigned int i = 0; i < ALL_GRADEABLES.size(); i++) {
      enum GRADEABLE_ENUM g = ALL_GRADEABLES[i];
      if (g == GRADEABLE_ENUM::NOTE) {
        assert (GRADEABLES[g].getPercent() < 0.01);
        continue;
      }

      float grade;
      if (this_student->getUserName() == "AVERAGE" ||
          this_student->getUserName() == "STDDEV") {
        // Special case for per gradeable average and standard deviation.
        // Mathematically, we can't simply add the std dev for the
        // different gradeables.  Note also: the average isn't a simple
        // addition either, since blank scores for a specific gradeable
        // are omitted from the average.
        std::vector<float> vals;
        for (unsigned int S = 0; S < students.size(); S++) {
          if (validSection(students[S]->getSection()) &&
              (!OmitSectionFromStats(students[S]->getSection()))) {
            vals.push_back(students[S]->GradeablePercent(g));
          }
        }
        float tmp_average = compute_average(vals);
        if (this_student->getUserName() == "AVERAGE") {
          grade = tmp_average;
        } else {
          float tmp_std_dev = compute_stddev(vals,tmp_average);
          grade = tmp_std_dev;
        }
      } else {
        grade = this_student->GradeablePercent(g);
      }

      std::string color = coloritcolor(grade,
                                       sp->GradeablePercent(g),
                                       sa->GradeablePercent(g),
                                       sb->GradeablePercent(g),
                                       sc->GradeablePercent(g),
                                       sd->GradeablePercent(g));
      if (this_student == STDDEV_STUDENT_POINTER) color="ffffff";
      assert (color.size()==6);
      table.set(myrow,counter++,TableCell(color,grade,2));
    }
    table.set(myrow,counter++,TableCell(grey_divider));
    }
    
    // ----------------------------
    // DETAILS OF EACH GRADEABLE    
    if (DISPLAY_GRADE_DETAILS) {
      for (unsigned int i = 0; i < ALL_GRADEABLES.size(); i++) {
        GRADEABLE_ENUM g = ALL_GRADEABLES[i];
        enum CELL_CONTENTS_STATUS visible = CELL_CONTENTS_VISIBLE;
        if (g == GRADEABLE_ENUM::TEST) {
          visible = CELL_CONTENTS_NO_DETAILS;
        }
        for (int j = 0; j < GRADEABLES[g].getCount(); j++) {
          float grade = this_student->getGradeableItemGrade(g,j).getValue();
          std::string color = coloritcolor(grade,
                                           sp->getGradeableItemGrade(g,j).getValue(),
                                           sa->getGradeableItemGrade(g,j).getValue(),
                                           sb->getGradeableItemGrade(g,j).getValue(),
                                           sc->getGradeableItemGrade(g,j).getValue(),
                                           sd->getGradeableItemGrade(g,j).getValue());
          if (this_student == STDDEV_STUDENT_POINTER) color="ffffff";
          std::string details;
          details = this_student->getGradeableItemGrade(g,j).getNote();
          std::string status = this_student->getGradeableItemGrade(g,j).getStatus();
          std::string event = this_student->getGradeableItemGrade(g,j).getEvent();
          bool Academic_integrity = this_student->getGradeableItemGrade(g,j).getAcademicIntegrity();
          std::string reason = this_student->getGradeableItemGrade(g,j).getReasonForException();
          std::string gID = GRADEABLES[g].getID(j);
          std::string userName = this_student->getUserName();
          if (status.find("Bad") != std::string::npos) {
            details += " " + status;
          }
          int late_days_used = this_student->getGradeableItemGrade(g,j).getLateDaysUsed();
          int daysExtended = this_student->getGradeableItemGrade(g,j).getLateDayExceptions();
          assert (color.size()==6);
          std::string a = "right";
          table.set(myrow,counter++,TableCell(grade,color,1,details,late_days_used,visible,event,Academic_integrity,a,1,0,reason,gID,userName,daysExtended));
        }
        table.set(myrow,counter++,TableCell(grey_divider));

        // FIXME
        if (g == GRADEABLE_ENUM::TEST && TEST_IMPROVEMENT_AVERAGING_ADJUSTMENT) {
          for (int j = 0; j < GRADEABLES[g].getCount(); j++) {
            float grade = this_student->adjusted_test(j);
            std::string color = coloritcolor(this_student->adjusted_test(j),
                                             sp->adjusted_test(j),
                                             sa->adjusted_test(j),
                                             sb->adjusted_test(j),
                                             sc->adjusted_test(j),
                                             sd->adjusted_test(j));
            if (this_student == STDDEV_STUDENT_POINTER) color="ffffff";
            assert (color.size()==6);
            table.set(myrow,counter++,TableCell(color,grade,1,"",0,visible));
          }
          table.set(myrow,counter++,TableCell(grey_divider));
        }
      }

      if (DISPLAY_LATE_DAYS) {
        // LATE DAYS
        if (this_student->getLastName() != "") {
          int allowed = this_student->getAllowedLateDays(100);
          int polls_correct = this_student->getPollsCorrect();
          int polls_incorrect = this_student->getPollsIncorrect();
          int total_polls = polls_correct + polls_incorrect;

          //total poll points, total answered polls, correct, incorrect
          //TODO: Add coloring?
          color="ffffff"; // default_color;
          table.set(myrow,counter++,TableCell(color,this_student->getPollPoints(),1,"",0,CELL_CONTENTS_VISIBLE,"right"));
          table.set(myrow,counter++,TableCell(color,total_polls,"",0,CELL_CONTENTS_VISIBLE,"right"));
          table.set(myrow,counter++,TableCell(color,polls_correct,"",0,CELL_CONTENTS_VISIBLE,"right"));
          table.set(myrow,counter++,TableCell(color,polls_incorrect,"",0,CELL_CONTENTS_VISIBLE,"right"));

          //std::string color = coloritcolor(allowed,5,4,3,2,2);
          //table.set(myrow,counter++,TableCell(color,allowed,"",0,CELL_CONTENTS_VISIBLE,"right"));
          //int used = this_student->getUsedLateDays();
          //color = coloritcolor(allowed-used+2, 5+2, 3+2, 2+2, 1+2, 0+2);
          //table.set(myrow,counter++,TableCell(color,used,"",0,CELL_CONTENTS_VISIBLE,"right"));
          //int exceptions = this_student->getLateDayExceptions();
          //color = coloritcolor(exceptions,5,4,3,2,2);
          //table.set(myrow,counter++,TableCell(color,exceptions,"",0,CELL_CONTENTS_VISIBLE,"right"));
        } else {
          color="ffffff"; // default_color;
          table.set(myrow,counter++,TableCell(color,""));
          table.set(myrow,counter++,TableCell(color,""));
          table.set(myrow,counter++,TableCell(color,""));
          table.set(myrow,counter++,TableCell(color,""));
          //table.set(myrow,counter++,TableCell(color,""));
          //table.set(myrow,counter++,TableCell(color,""));
          //table.set(myrow,counter++,TableCell(color,""));
        }
        table.set(myrow,counter++,TableCell(grey_divider));
      }
    }
  }


  
  for (int i = 0; i < table.numCols(); i++) {
    instructor_data.push_back(i);
  }
  // need to add 2, for the perfect & average student.
  for (unsigned int i = 0; i < students.size()+2; i++) {
    all_students.push_back(i);
  }

  if(!csv_mode) {
      std::cout << "WRITE ALL.html" << std::endl;
      std::ofstream ostr_html(OUTPUT_FILE);

      GLOBAL_instructor_output = true;
      table.output(ostr_html, all_students, instructor_data, csv_mode);

      end_table(ostr_html, true, NULL);
      ostr_html.close();
  }
  else {
      std::cout << "WRITE ALL.csv" << std::endl;
      std::ofstream ostr_csv(OUTPUT_CSV_FILE);

      GLOBAL_instructor_output = true;
      table.output(ostr_csv, all_students, instructor_data, csv_mode);

      ostr_csv.close();
  }

  std::stringstream ss;

  if(!csv_mode) {
      ss << ALL_STUDENTS_OUTPUT_DIRECTORY << "output_" << month << "_" << day << "_" << year << ".html";
      std::string command = "cp -f output.html " + ss.str();
      std::cout << "RUN COMMAND " << command << std::endl;
      system(command.c_str());
  }
  else{
      ss << ALL_STUDENTS_OUTPUT_DIRECTORY_CSV << "output_" << month << "_" << day << "_" << year << ".csv";
      std::string command = "cp -f output.csv " + ss.str();
      std::cout << "RUN COMMAND " << command << std::endl;
      system(command.c_str());
  }
  

  for (std::map<int,std::string>::iterator itr = student_correspondences.begin();
       itr != student_correspondences.end(); itr++) {

    select_students[2] = itr->first;
    std::string filename = "individual_summary_html/" + itr->second + "_summary.html";
    std::ofstream ostr3(filename.c_str());
    assert (ostr3.good());

    Student *s = GetStudent(students,itr->second);
    std::string last_update;
    if (s != NULL) {
      last_update = s->getLastUpdate();
    }
    GLOBAL_instructor_output = false;

    table.output(ostr3, select_students,student_data, false,true,true,last_update);

    end_table(ostr3,false,s);
  }

  Student* s = NULL;
  if (rank != -1) {
    s = students[rank];
    assert (s != NULL);
  }
}




void end_table(std::ofstream &ostr,  bool for_instructor, Student *s) {

  ostr << "<p>* = 1 late day used</p>" << std::endl;


  bool print_academic_sanction_message = false;
  if (s != NULL && s->getAcademicSanctionPenalty() < -0.0000001) {
    print_academic_sanction_message = true;
  }

  if (print_academic_sanction_message) {
    ostr << "@ = Academic Integrity Violation penalty<p>&nbsp;<p>\n";
  }

  // Description of border outline that are in effect
  if (for_instructor || s != NULL)
  {
      if (for_instructor || (s != NULL && (s->get_event_bad_status() || s->get_event_grade_inquiry() || s->get_event_overridden() || s->get_event_academic_integrity() || s->get_event_extension())))
      {
        ostr << "<style> .spacer {display: inline-block; width: 66px;} </style>\n";
        ostr << "<table style=\"border:1px solid #aaaaaa; background-color:#FFFFFF;\">\n";
        if (for_instructor || (s != NULL && s->get_event_academic_integrity()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px solid #0a0a0a; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Academic Integrity Violation </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
        if (for_instructor || (s != NULL && s->get_event_overridden()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px solid #fcca03; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Grade Override </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
        if (for_instructor || (s != NULL && s->get_event_extension()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px solid #0066e0; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Excused Absence Extension </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
        if (for_instructor || (s != NULL && s->get_event_grade_inquiry()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px dashed #1cfc03; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Grade Inquiry in Progress </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
       if (for_instructor || (s != NULL && s->get_event_cancelled()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px dashed #0a0a0a; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Cancelled submission </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
       if (for_instructor || (s != NULL && s->get_event_version_conflict()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px dashed #fc0303; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Version conflict = version conflict between <br> ";
          ostr << "<span class=\"spacer\"></span> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;TA graded version and Active version  </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
        if (for_instructor || (s != NULL && s->get_event_bad_status()))
        {
          ostr << "<tr>\n";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << "outline:4px solid #fc0303; outline-offset: -4px;" << " \" align=\"" << "left" << "\">";
          ostr << "<span class=\"spacer\"></span>";
          ostr << "</td>";
          ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#FFFFFF" << "; " << " \" align=\"" << "left" << "\">";
          ostr << "<font size = \"-1\"> Bad status = too many late days used on this assignment <br> ";
          ostr << "<span class=\"spacer\"></span> OR you didn’t have enough late days to use </font>";
          ostr << "</td>";
          ostr << "</tr>\n";
        }
        ostr << "</table>\n";
      }
  }


  if (s != NULL) {
    std::ifstream istr2("student_extension_reports/"+s->getUserName()+".html");
    if (istr2.good()) {
      std::string tmp_s;
      while (getline(istr2,tmp_s)) {
        ostr << tmp_s;
      }
    }
    std::ifstream istr("student_poll_reports/"+s->getUserName()+".html");
    if (istr.good()) {
      std::string tmp_s;
      while (getline(istr,tmp_s)) {
        ostr << tmp_s;
      }
    }
  }
  ostr << "<p>&nbsp;<p>\n";


  if (DISPLAY_FINAL_GRADE) { // && students.size() > 50) {

  int total_A = grade_counts[Grade("A")] + grade_counts[Grade("A-")];
  int total_B = grade_counts[Grade("B+")] + grade_counts[Grade("B")] + grade_counts[Grade("B-")]; 
  int total_C = grade_counts[Grade("C+")] + grade_counts[Grade("C")] + grade_counts[Grade("C-")];
  int total_D = grade_counts[Grade("D+")] + grade_counts[Grade("D")];
  int total_passed = total_A + total_B + total_C + total_D;
  int total_F = grade_counts[Grade("F")];
  int total_blank = grade_counts[Grade("")];
  assert (total_blank == 0);
  int total = total_passed + total_F + auditors + total_blank + dropped;

  ostr << "<p>\n";



  ostr << "<table style=\"border:1px solid yellowgreen; background-color:#ddffdd; width:auto;\">\n";
  //  ostr << "<table border=2 cellpadding=5 cellspacing=0>\n";
  ostr << "<tr>\n";
  ostr << "<td width=150>FINAL GRADE</td>";
  ostr << "<td align=center bgcolor="<<GradeColor("A")<<" width=40>A</td><td align=center bgcolor="<<GradeColor("A-")<<" width=40>A-</td>";
  ostr << "<td align=center bgcolor="<<GradeColor("B+")<<" width=40>B+</td><td align=center bgcolor="<<GradeColor("B")<<" width=40>B</td><td align=center bgcolor="<<GradeColor("B-")<<" width=40>B-</td>";
  ostr << "<td align=center bgcolor="<<GradeColor("C+")<<" width=40>C+</td><td align=center bgcolor="<<GradeColor("C")<<" width=40>C</td><td align=center bgcolor="<<GradeColor("C-")<<" width=40>C-</td>";
  ostr << "<td align=center bgcolor="<<GradeColor("D+")<<" width=40>D+</td><td align=center bgcolor="<<GradeColor("D")<<" width=40>D</td>\n";
  if (for_instructor) {
    ostr << "<td align=center bgcolor="<<GradeColor("F")<<"width=40>F</td>\n";
    //    ostr << "<td align=center width=40>dropped</td>\n";
    ostr << "<td align=center width=40>audit</td>\n";
    ostr << "<td align=center align=center width=40>took final</td>\n";
    ostr << "<td align=center align=center width=40>total passed</td>\n";
    ostr << "<td align=center align=center width=40>dropped</td>\n";
    ostr << "<td align=center align=center width=40>total</td>\n";
  }
  ostr << "</tr>\n";
  
  ostr << "<tr>\n";
  ostr << "<td width=150># of students</td>";
  ostr << "<td align=center width=40>"<<grade_counts[Grade("A")]<<"</td><td align=center width=40>"<<grade_counts[Grade("A-")]<<"</td>";
  ostr << "<td align=center width=40>"<<grade_counts[Grade("B+")]<<"</td><td align=center width=40>"<<grade_counts[Grade("B")]<<"</td><td align=center width=40>"<<grade_counts[Grade("B-")]<<"</td>";
  ostr << "<td align=center width=40>"<<grade_counts[Grade("C+")]<<"</td><td align=center width=40>"<<grade_counts[Grade("C")]<<"</td><td align=center width=40>"<<grade_counts[Grade("C-")]<<"</td>";
  ostr << "<td align=center width=40>"<<grade_counts[Grade("D+")]<<"</td><td align=center width=40>"<<grade_counts[Grade("D")]<<"</td>\n";
  
  if (for_instructor) {
    ostr << "<td align=center width=40>"<<grade_counts[Grade("F")]<<"</td>\n";
    //ostr << "<td align=center width=40>" << grade_counts[Grade("")]<<"</td>\n";
    ostr << "<td align=center width=40>"<<auditors<<"</td>\n";
    ostr << "<td align=center width=40>"<<took_final<<"</td>\n";
    ostr << "<td align=center width=40>"<<total_passed<<"</td>\n";
    ostr << "<td align=center width=40>"<<dropped<<"</td>\n";
    ostr << "<td align=center width=40>"<<total<<"</td>\n";
  }
  ostr << "</tr>\n";
  
  
  
  ostr << "<tr>\n";
  ostr << "<td width=150>average OVERALL<br>of students with<br>this FINAL GRADE</td>";
  ostr << "<td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("A")]<<"</td><td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("A-")]<<"</td>";
  ostr << "<td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("B+")]<<"</td><td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("B")]<<"</td><td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("B-")]<<"</td>";
  ostr << "<td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("C+")]<<"</td><td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("C")]<<"</td><td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("C-")]<<"</td>";

  if (for_instructor) {
    ostr << "<td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("D+")]<<"</td><td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("D")]<<"</td>\n";
  } else {
    ostr << "<td align=center width=40> &nbsp; </td><td align=center width=40> &nbsp; </td>\n";
  }

  if (for_instructor) {
    ostr << "<td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("F")]<<"</td>\n";
    //ostr << "<td align=center width=40>"<<std::setprecision(1)<<std::fixed<<grade_avg[Grade("")]<<"</td>\n";
    ostr << "<td align=center width=40>&nbsp;</td>\n";
    ostr << "<td align=center width=40>&nbsp;</td>\n";
    ostr << "<td align=center width=40>&nbsp;</td>\n";
    ostr << "<td align=center width=40>&nbsp;</td>\n";
    ostr << "<td align=center width=40>&nbsp;</td>\n";
  }
  ostr << "</tr>\n";
  
  
  
  ostr << "</table><p>\n";

  }

  // RAINBOW GRADE VERSION DISPLAY
  std::ifstream istr(RG_VERSION_FILE.c_str());
  assert (istr.good());
  nlohmann::json j = nlohmann::json::parse(istr);
  // current_commit_hash_rg (Full hash) is not used, parsing just for possible future use
  nlohmann::json current_commit_hash_rg = j["installed_commit_rg"];
  nlohmann::json current_short_commit_hash_rg = j["short_installed_commit_rg"];
  nlohmann::json current_git_tag_rg = j["most_recent_git_tag_rg"];

  // Remove double quotation
  std::string rainbow_short_hash = current_short_commit_hash_rg;
  std::string rainbow_git_tag = current_git_tag_rg;
  rainbow_git_tag.erase(std::remove(rainbow_git_tag.begin(), rainbow_git_tag.end(), '\"'), rainbow_git_tag.end());
  rainbow_short_hash.erase(std::remove(rainbow_short_hash.begin(), rainbow_short_hash.end(), '\"'), rainbow_short_hash.end());

  std::time_t currentTime = std::time(nullptr);
  // Convert the time to the local time struct
  std::tm* localTime = std::localtime(&currentTime);
  // Extract the current year from the local time struct
  int currentYear = localTime->tm_year + 1900;


  ostr << "<p>&copy; " << currentYear << " <a href=\"https://submitty.org/instructor/course_settings/rainbow_grades/index\" target=\"_blank\" class=\"black-btn\">Submitty/RainbowGrades</a> <a href=\"https://github.com/Submitty/RainbowGrades/releases/tag/" << rainbow_git_tag
               << "\" target=\"_blank\" title=\"" << rainbow_git_tag << " " << rainbow_short_hash
               << "\" class=\"black-btn\">" << rainbow_git_tag << "</a></p>" << std::endl;




  ostr.close();
}
