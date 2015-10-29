#include "db/db_help.h"

#include <iostream>
using namespace std;

namespace db {
int DBHelp() {
  cout << endl << "Usage: minisql [script] [options]" << endl << endl;
  cout << "minisql             Run in REPL mode." << endl;
  cout << "minisql [script]    Run SQL script." << endl;
  cout << "minisql --help      Quick help." << endl;
  cout << "minisql --debug     Output debug info." << endl;
  return 0;
}
}  // namespace db
