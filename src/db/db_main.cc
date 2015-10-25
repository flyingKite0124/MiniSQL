#include "src/base/command_line.h"
#include "src/db/db_main.h"
#include "src/db/db_help.h"

#include <stdio.h>
using namespace std;

int main(int argc, const char* argv[]) {
  auto command_line = Base::CommandLine::ParseCommandLine(argc, argv);
  for (auto& command: command_line) {
    printf("%s: %s\n", command.first.c_str(),
                       command.second.c_str());
  }
  if (command_line.find("help") != command_line.end()) {
    // `--help` for usage.
    return Database::DBHelp();
  } else if (command_line.find("script") != command_line.end()) {
    // A filename following for script execution.
    // return DB::DBRunScript(command_line[filename]);
  } else {
    // return DB::DBREPL();
  }
  return 0;
}
