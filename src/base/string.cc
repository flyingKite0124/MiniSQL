#include "base/string.h"
using namespace std;

namespace base {
namespace String {
string Trim(string s) {
  string trimmed = "";
  bool isSpace = true;
  size_t string_end = s.length();
  while (string_end > 0 && s[string_end - 1] == ' ')
    --string_end;
  for (size_t i = 0; i < string_end; ++i) {
    if (isSpace && s[i] == ' ')
      continue;
    trimmed += s[i];
    isSpace = s[i] == ' ';
  }
  return trimmed;
}
vector<string> Split(string s, char splitter) {
  vector<string> res;
  string cur = "";
  for (size_t i = 0; i < s.length(); ++i) {
    if (s[i] == splitter) {
      res.push_back(cur);
      cur = "";
    }
    else
      cur += s[i];
  }
  res.push_back(cur);
  return res;
}
string TakeOffBracket(string s) {
  size_t bracket_left = s.find('('),
         bracket_right = s.rfind(')');
  if (bracket_left == string::npos ||
      bracket_right == string::npos ||
      bracket_left > bracket_right)
    throw new string("Unmatched brackets in table definition.");
  return s.substr(bracket_left + 1, bracket_right - bracket_left - 1);
}
int ToInt(string s) {
  int res = 0;
  for (size_t i = 0; i < s.length(); ++i)
    if (s[i] >= '0' && s[i] <= '9')
      res = res * 10 + s[i] - '0';
    else
      throw string("String `" + s + "` is not a valid integer.");
  return res;
}
}  // namespace String
}  // namespace base
