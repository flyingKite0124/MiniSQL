#include "db/db_help.h"
#include "stdio.h"

namespace db {
int DBHelp() {
  puts("\nUsage: minisql [script] [--help]\n");
  puts("minisql --help      Quick help.");
  puts("minisql             Run in REPL mode.");
  puts("minisql [script]    Run SQL script.");
  return 0;
}
}  // namespace db
