#include <cmath>
#include <cassert>

#include "table.h"
#include "constants_and_globals.h"

bool GLOBAL_instructor_output = false;

bool global_details = false;

// XXX: For now not sanitizing \ since RFC4180 only specifies double quote as an escape
//      i.e. """ is a field with one double quote as the value.
//      In practice many other escape techniques are used, but we'll see how far this gets us
//      and we can always add the \ into the sanitization list if users want it.
//      Also ' is not part of RFC4180 so not stripping those for now either. Probably going
//      to result in some strange behavior in some spreadsheet apps, need user testing.
//      We could relax the stripping, allow commas, and ensure all output uses double quotes
//      to wrap all fields. However, this would make using a text editor to read/edit the CSV painful.
bool CSVSanitizeHelper(const char c){
    return c == '"' || c == ',' || c == '\n' || c == '\r';
}

std::string CSVSanitizeString(const std::string& s){
    std::string ret(s);
    ret.erase(std::remove_if(ret.begin(), ret.end(), CSVSanitizeHelper), ret.end());
    return ret;
}

TableCell::TableCell(const std::string& c, const std::string& d, const std::string& n, int ldu,
                     CELL_CONTENTS_STATUS v, const std::string& a, int s, int r) { 
  assert (c.size() == 6);
  color=c; 
  data=d; 
  note=n; 
  late_days_used=ldu,
  visible=v;
  align=a;
  span=s; 
  rotate=r;
}

TableCell::TableCell(const std::string& c, int d, const std::string& n, int ldu,
                     CELL_CONTENTS_STATUS v, const std::string& a, int s, int r) { 
  assert (c.size() == 6);
  color=c; 
  data=std::to_string(d); 
  note=n; 
  late_days_used=ldu,
  visible=v;
  align=a;
  span=s; 
  rotate=r;
}

TableCell::TableCell(const std::string& c, float d, int precision, const std::string& n, int ldu,
                     CELL_CONTENTS_STATUS v, const std::string& a, int s, int /*r*/) {
  assert (c.size() == 6);
  assert (precision >= 0);
  color=c; 
  if (fabs(d) > 0.0001) {
    std::stringstream ss;
    ss << std::setprecision(precision) << std::fixed << d;
    data=ss.str(); span=s; 
  } else {
    data = "";
  }
  note=n;
  late_days_used=ldu,
  visible=v;
  align=a;
  span=s; 
  rotate = 0;
}

TableCell::TableCell(float d, const std::string& c, int precision, const std::string& n, int ldu,
                     CELL_CONTENTS_STATUS v,const std::string& e,bool ai, const std::string& a, 
                     int s, int /*r*/,const std::string& reason,const std::string& gID,const std::string& userName, 
                     int daysExtended, float penalty_per_day, bool automatic_zero) {
  assert (c.size() == 6);
  assert (precision >= 0);
  color=c;
  
  // Store the original score before penalties
  float original_score = d;
  
  // Apply penalty if not an automatic zero
  if (automatic_zero) {
      d = 0; // Set final score to 0 if automatic zero
  } else if (late_days_used > 0) {
        // Apply penalty if not an automatic zero
  if (automatic_zero) {
      d = 0; // Set final score to 0 if automatic zero
  } else if (late_days_used > 0) {
      d = d * (1.0f - penalty_per_day * late_days_used); // Apply percentage deduction for late days
  }
  }
  
  // Ensure numeric precision
  if (fabs(d) > 0.0001) {
    std::stringstream ss;
    ss << std::setprecision(precision) << std::fixed << d;
    data=ss.str(); span=s;
  } else {
    data = "";
  }
  note=n;
  late_days_used=ldu;
  visible=v;
  align=a;
  span=s;
  rotate = 0;
  academic_integrity = ai;
  event = e;
  
  // Generating hover text with penalty information or automatic zero
  if (event == "Overridden") {
    override = true;
    bad_status = inquiry = extension = version_conflict = cancelled = false;
  } else if (event == "Extension") {
    extension = true;
    inquiry = bad_status = override = version_conflict = cancelled = false;
    hoverText = "class=\"hoverable-cell\" data-hover-text=\"" + CSVSanitizeString(userName) + 
                " received a " + std::to_string(daysExtended) + " day extension\" ";
  } else if (event == "Bad") {
    bad_status = true;
    override = inquiry = extension = version_conflict = cancelled = false;
    if (automatic_zero) {
        hoverText = "class=\"hoverable-cell\" data-hover-text=\"" + CSVSanitizeString(userName) +
                    " received an automatic zero due to late submission.\" ";
    } else {
        hoverText = "class=\"hoverable-cell\" data-hover-text=\"" + CSVSanitizeString(userName) +
                    " received a bad status on " + CSVSanitizeString(gID) + ". Original score: " +
                    std::to_string(original_score) + ", Final score after " + std::to_string(late_days_used) +
                    " late days and a penalty of " + std::to_string(penalty_per_day * 100) + "% per day: " + data + "\" ";
    }

  }
}


std::ostream& operator<<(std::ostream &ostr, const TableCell &c) {
  assert (c.color.size() == 6);
    
    std::string outline = "";
    std::string mark = "";
    if (c.academic_integrity){
        outline = "outline:4px solid #0a0a0a; outline-offset: -4px;";
        mark = "@";
    } else if (c.override){
        outline = "outline:4px solid #fcca03; outline-offset: -4px;";
    } else if (c.extension){
        outline = "outline:4px solid #0066e0; outline-offset: -4px;";
    } else if (c.inquiry){
        outline = "outline:4px dashed #1cfc03; outline-offset: -4px;";
    } else if (c.cancelled){
        outline = "outline:4px dashed #0a0a0a; outline-offset: -4px;";
    } else if (c.version_conflict){
        outline = "outline:4px dashed #fc0303; outline-offset: -4px;";
    } else if (c.bad_status){
        outline = "outline:4px solid #fc0303; outline-offset: -4px;";
    }

    // Render with hover text for bad_status or extension
    if (c.extension || c.bad_status) {
        ostr << "<td " << c.hoverText << "style=\"border:1px solid #aaaaaa; background-color:#" << c.color << "; " << outline << "\" align=\"" << c.align << "\">";
    } else {
        ostr << "<td style=\"border:1px solid #aaaaaa; background-color:#" << c.color << "; " << outline << "\" align=\"" << c.align << "\">";
    }

    // Rendering the content of the cell
    if (0) { //rotate == 90) {
      ostr << "<div style=\"position:relative\"><p class=\"rotate\">";
    }
    ostr << "<font size=-1>";
    std::string mynote = c.getNote();

    if (c.academic_integrity)
    {
      ostr << mark;
    }
    else if ((c.data == "" && mynote=="")
        || c.visible==CELL_CONTENTS_HIDDEN
        || (c.visible==CELL_CONTENTS_VISIBLE_INSTRUCTOR && GLOBAL_instructor_output == false) 
        || (c.visible==CELL_CONTENTS_VISIBLE_STUDENT    && GLOBAL_instructor_output == true)) {
      ostr << "<div></div>";
    } else {
      ostr << c.data; 
      if (c.late_days_used > 0) {
        if (c.late_days_used > 3) { ostr << " (" << std::to_string(c.late_days_used) << "*)"; }
        else { ostr << " " << std::string(c.late_days_used,'*'); }
      }
        
      if (mynote.length() > 0 &&
          mynote != " " &&
          global_details) {
        ostr << "<br><em>" << mynote << "</em>";
      }
    }
    ostr << "</font>";
    if (0) { //rotate == 90) {
      ostr << "</p></div>";
    }
    ostr << "</td>";
    return ostr;
}


std::string TableCell::make_cell_string(bool csv_mode) const{
    std::string ret;
    std::string mynote = this->getNote();
    if ((this->data == "" && mynote=="")
        || this->visible==CELL_CONTENTS_HIDDEN
        || (this->visible==CELL_CONTENTS_VISIBLE_INSTRUCTOR && GLOBAL_instructor_output == false)
        || (this->visible==CELL_CONTENTS_VISIBLE_STUDENT    && GLOBAL_instructor_output == true)) {
        //ret += "\n"; //Empty cell
    } else {
        ret += this->data;
        if (this->late_days_used > 0 && csv_mode == false) {
            if (this->late_days_used > 3) { ret += " (" + std::to_string(this->late_days_used) + "*)"; }
            else { ret += " " + std::string(this->late_days_used,'*'); }
        }
        if (mynote.length() > 0 &&
            mynote != " " &&
            global_details) {
            ret += mynote;
            std::cerr << "Printing note: " << mynote << std::endl;
        }
    }
    return CSVSanitizeString(ret);
}

bool TableCell::hasRedOutline() const {
    return red_outline;
}

bool TableCell::isAutomaticZero() const {
    return automatic_zero;
}

bool TableCell::hasPenalty() const {
    return penalty;
}

float TableCell::getOriginalScore() const {
    return original_score;
}

float TableCell::getScore() const {
    return score;
}

std::string TableCell::getData() const {
    return data;
}

void Table::output(std::ostream& ostr,
                   std::vector<int> which_students,
                   std::vector<int> which_data,
                   bool csv_mode,
                   bool transpose,
                   bool show_details,
                   std::string last_update) const {

  global_details = show_details;
  //global_details = true;

  if(!csv_mode) {
      ostr << "<style>\n";
      ostr << ".rotate {\n";
      ostr << "             filter:  progid:DXImageTransform.Microsoft.BasicImage(rotation=0.083);  /* IE6,IE7 */\n";
      ostr << "         -ms-filter: \"progid:DXImageTransform.Microsoft.BasicImage(rotation=0.083)\"; /* IE8 */\n";
      ostr << "     -moz-transform: rotate(-90.0deg);  /* FF3.5+ */\n";
      ostr << "      -ms-transform: rotate(-90.0deg);  /* IE9+ */\n";
      ostr << "       -o-transform: rotate(-90.0deg);  /* Opera 10.5 */\n";
      ostr << "  -webkit-transform: rotate(-90.0deg);  /* Safari 3.1+, Chrome */\n";
      ostr << "          transform: rotate(-90.0deg);  /* Standard */\n";
      ostr << " display:block;\n";
      ostr << " position:absolute;\n";
      ostr << " right:-50%;\n";
      ostr << "}\n";
      ostr << "</style>\n";
  }


  // -------------------------------------------------------------------------------
  // PRINT INSTRUCTOR SUPPLIED MESSAGES
  if(!csv_mode) {
      for (unsigned int i = 0; i < MESSAGES.size(); i++) {
          ostr << "" << MESSAGES[i] << "<br>\n";
      }
      if (last_update != "") {
          ostr << "<em>Information last updated: " << last_update << "</em><br>\n";
      }

      ostr << "<style>";
      ostr << ".hoverable-cell {";
      ostr << "    position: relative;";
      ostr << "}";
      ostr << ".hoverable-cell:hover::before {";
      ostr << "    content: attr(data-hover-text);";
      ostr << "    position: absolute;";
      ostr << "    text-align: left;";
      ostr << "    left: 50%;";
      ostr << "    bottom: 85%;";
      ostr << "    width: 500%;";
      ostr << "    height: auto;";
      ostr << "    background-color: rgba(255, 255, 255, 0.88);"; // semi-opaque white background
      ostr << "    padding: 5px;";
      ostr << "    border: 1px solid #aaa;";
      ostr << "    z-index: 1;";
      ostr << "    display: flex;";
      ostr << "    align-items: left;";
      ostr << "    justify-content: left;";
      ostr << "    box-sizing: border-box;";
      ostr << "}";
      ostr << "</style>";


      ostr << "&nbsp;<br>\n";
      ostr << "<table style=\"border:1px solid #aaaaaa; background-color:#aaaaaa;\">\n";
  }


// -------------------------------------------------------------------------------
// ADD HOVER TEXT FOR CELLS WITH RED OUTLINE
for (const auto& row : cells) {
    for (const auto& cell : row) {
        std::string hover_text;
        if (cell.hasRedOutline()) {
            if (cell.isAutomaticZero()) {
                hover_text = "This assignment received an automatic zero because it was submitted too late.";
            } else if (cell.hasPenalty()) {
                hover_text = "This assignment received a grade penalty for being late. Original score: " + std::to_string(cell.getOriginalScore()) + ", Score after penalty: " + std::to_string(cell.getScore());
            }
            ostr << "<td class='hoverable-cell red-outline' data-hover-text='" << hover_text << "'>" << cell.getData() << "</td>\n";
        } else {
            ostr << "<td>" << cell.getData() << "</td>\n";
        }
    }
}


  if (transpose) {
    for (std::vector<int>::iterator c = which_data.begin(); c != which_data.end(); c++) {
      ostr << "<tr>\n";
      for (std::vector<int>::iterator r = which_students.begin(); r != which_students.end(); r++) {
        ostr << cells[*r][*c] << "\n";
      }
      ostr << "</tr>\n";
    }
  } else {
      //CSV only runs in transpose = false
      if(!csv_mode){
          for (std::vector<int>::iterator r = which_students.begin(); r != which_students.end(); r++) {
              ostr << "<tr>\n";
              for (std::vector<int>::iterator c = which_data.begin(); c != which_data.end(); c++) {
                  ostr << cells[*r][*c] << "\n";
              }
              ostr << "</tr>\n";
          }
      }
      else{
          for (std::vector<int>::iterator r = which_students.begin(); r != which_students.end(); r++) {
              bool first_cell = true;
              for (std::vector<int>::iterator c = which_data.begin(); c != which_data.end(); c++) {
                  if(first_cell){
                      first_cell = false;
                  }
                  else{
                      ostr << ",";
                  }
                  ostr << cells[*r][*c].make_cell_string(csv_mode);
              }
              ostr << "\n";
          }
      }
  } 

  if(!csv_mode) {
      ostr << "</table>" << std::endl;
  }
}
