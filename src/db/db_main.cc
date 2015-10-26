#include "base/command_line.h"
#include "db/db_main.h"
#include "db/db_help.h"

#include <stdio.h>
using namespace std;

int main(int argc, const char* argv[]) {
  auto command_line = base::CommandLine::ParseCommandLine(argc, argv);
  if (command_line.find("help") != command_line.end()) {
    // `--help` for usage.
    return db::DBHelp();
  } else if (command_line.find("script") != command_line.end()) {
    // A filename following for script execution.
    // return DB::DBRunScript(command_line[filename]);
  } else {
    // return DB::DBREPL();
  }
  return 0;
}
