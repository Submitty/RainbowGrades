#include "section.h"
#include "constants_and_globals.h"

#include <nlohmann/json.hpp>

bool validSection(std::string section,
                  nlohmann::json &json) {
  nlohmann::json::iterator itr = json.find("section");
  assert(itr != json.end());
  assert(itr->is_object());

  nlohmann::json::iterator itr2 = itr->find(section);
  if (itr2 == itr->end())
    return false;
  return true;
}

std::string sectionName(std::string section) {
  std::map<std::string, std::string>::const_iterator itr =
      sectionNames.find(section);
  if (itr == sectionNames.end())
    return "NONE";
  return itr->second;
}

bool OmitSectionFromStats(const std::string &section) {
  for (std::vector<std::string>::size_type i = 0; i < OMIT_SECTION_FROM_STATS.size(); i++) {
    if (OMIT_SECTION_FROM_STATS[i] == section) return true;
  }
  return false;
}
