#include "db/db_help.h"

#include <iostream>
using namespace std;

namespace db {
int DBHelp() {
  cout << endl << "Usage: minisql [script] [--help]" << endl << endl;
  cout << "minisql --help      Quick help." << endl;
  cout << "minisql             Run in REPL mode." << endl;
  cout << "minisql [script]    Run SQL script." << endl;
  return 0;
}
}  // namespace db
