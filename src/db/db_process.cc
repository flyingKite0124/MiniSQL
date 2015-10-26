#include "db/db_process.h"

#include <iostream>
using namespace std;

namespace db {
int DBProcess(string command) {
  cout << "Process: " << command << endl;
  return 0;
}
}  // namespace db
