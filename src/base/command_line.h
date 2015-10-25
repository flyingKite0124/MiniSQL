#ifndef SRC_BASE_COMMAND_LINE_H
#define SRC_BASE_COMMAND_LINE_H

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

namespace Base {
namespace CommandLine {
std::map<std::string, std::string> ParseCommandLine(int argc,
                                                    const char* argv[]);
} // namespace CommandLine
} // namespace base

#endif
