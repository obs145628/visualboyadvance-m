#include "strutils.h"
#include <sstream>

namespace obs {

namespace {
bool is_wspace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
} // namespace

std::string str_trim(const std::string &str) {

  std::size_t bpos = 0;
  while (bpos < str.length() && is_wspace(str[bpos]))
    ++bpos;
  if (bpos == str.length())
    return "";

  std::size_t epos = str.length() - 1;
  while (is_wspace(str[epos]))
    --epos;

  return std::string(str.begin() + bpos, str.begin() + epos + 1);
}

std::vector<std::string> str_split(const std::string &str, char sep) {
  std::vector<std::string> res;
  std::istringstream is(str);
  std::string val;
  while (std::getline(is, val, sep))
    res.push_back(val);
  return res;
}

} // namespace obs
