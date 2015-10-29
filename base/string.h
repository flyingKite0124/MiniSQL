#ifndef BASE_STRING_H
#define BASE_STRING_H
#include <string>
#include <vector>

namespace base {
namespace String {

std::string Trim(std::string s);

std::vector<std::string> Split(std::string s, char splitter);

std::vector<std::string> Split(std::string s, std::string splitter);

std::string TakeOffBracket(std::string s);

int ToInt(std::string s);

std::string LiteralToString(std::string s);

bool IsWord(std::string s);

}  // namespace String
}  // namespace base

#endif  // BASE_STRING_H
