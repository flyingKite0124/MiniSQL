#include "db/db_main.h"

#include "base/io.h"
#include "base/command_line.h"
#include "db/db_help.h"
#include "db/db_repl.h"

#include <iostream>
#include <fstream>
using namespace std;

int db::debug = 0;

int main(int argc, const char* argv[]) {
  auto command_line = base::CommandLine::ParseCommandLine(argc, argv);
  if (command_line.find("debug") != command_line.end()) {
    db::debug = 1;
  }
  if (command_line.find("help") != command_line.end()) {
    // `--help` for usage.
    return db::DBHelp();
  } else if (command_line.find("script") != command_line.end()) {
    // A filename following for script execution.
    ifstream fin(command_line["script"]);
    return db::DBREPL(false, fin);
  } else {
    base::IO::InitializeHistory();
    int ret = db::DBREPL(false);
    base::IO::SaveHistory();
    return ret;
  }
  return 0;
}
