#ifndef BASE_COMMAND_LINE_H
#define BASE_COMMAND_LINE_H

#include <map>
#include <vector>
#include <string>
#include <algorithm>

namespace base {
namespace CommandLine {
std::map<std::string, std::string> ParseCommandLine(int argc,
                                                    const char* argv[]);
} // namespace CommandLine
} // namespace base

#endif  // BASE_COMMAND_LINE_H
