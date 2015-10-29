#include "base/command_line.h"
using namespace std;

namespace base {
namespace CommandLine {
map<string, string> ParseCommandLine(int argc, const char* argv[]) {
  vector<string> args;
  map<string, string> command_line;
  command_line.insert(make_pair("command", string(argv[0])));
  for (int i = 1; i < argc; ++i) {
    args.push_back(string(argv[i]));
  }
  for (auto& arg: args) {
    if (arg.substr(0, 2) == "--") {
      command_line.insert(make_pair(arg.substr(2), "true"));
    } else {
      command_line.insert(make_pair("script", arg));
    }
  }
  return command_line;
}
} // namespace CommandLine
} // namespace base
