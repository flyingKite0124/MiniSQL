#include "base/string.h"

#include <ctype.h>
#include <stdexcept>
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
vector<string> Split(string s, string splitter) {
  vector<string> res;
  string cur = "";
  size_t pos;
  while ((pos = s.find(splitter)) != string::npos) {
    if (pos == 0)
      res.push_back("");
    else
      res.push_back(s.substr(0, pos));
    if (pos + splitter.length() < s.length())
      s = s.substr(pos + splitter.length());
    else
      s = "";
  }
  res.push_back(s);
  return res;
}
string TakeOffBracket(string s) {
  size_t bracket_left = s.find('('),
         bracket_right = s.rfind(')');
  if (bracket_left == string::npos ||
      bracket_right == string::npos ||
      bracket_left > bracket_right)
    throw invalid_argument("Unmatched brackets in table definition.");
  return s.substr(bracket_left + 1, bracket_right - bracket_left - 1);
}
int ToInt(string s) {
  int res = 0;
  for (size_t i = 0; i < s.length(); ++i)
    if (s[i] >= '0' && s[i] <= '9')
      res = res * 10 + s[i] - '0';
    else
      throw invalid_argument("String `" + s + "` is not a valid integer.");
  return res;
}
string LiteralToString(string s) {
  if (s[0] == '\'' && s[s.length() - 1] == '\'') {
    string ret = s.substr(1, s.length() - 2);
    // TODO: escaping
    return ret;
  } else if (s[0] == '\"' && s[s.length() - 1] == '"') {
    string ret = s.substr(1, s.length() - 2);
    // TODO: escaping
    return ret;
  } else {
    return s;
  }
}
bool IsWord(string s) {
  if (s.length() > 32)
    return false;
  if (s.length() == 0 || !isalpha(s[0]))
    return false;
  for (size_t i = 0; i < s.length(); ++i)
    if (!isalnum(s[i]) && s[i] != '_')
      return false;
  return true;
}
}  // namespace String
}  // namespace base
