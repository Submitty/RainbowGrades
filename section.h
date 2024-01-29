#ifndef PROCESS_GRADES__SECTION_H
#define PROCESS_GRADES__SECTION_H
#include <string>
#include "constants_and_globals.h"

bool validSection(std::string section,
                  nlohmann::json &json = GLOBAL_CUSTOMIZATION_JSON) ;
std::string sectionName(std::string section);
bool OmitSectionFromStats(const std::string &section);


#endif // PROCESS_GRADES__SECTION_H
