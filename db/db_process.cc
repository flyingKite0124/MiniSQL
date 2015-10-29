#include "db/db_process.h"

#include <iostream>
using namespace std;

#include "db/db_main.h"
#include "db/db_type.h"

namespace db {
int DBProcess(string command) {
  Operation* op = NULL;
  DEBUG << "Processing: " << command << endl;
  try {
    if (command.find("create table ") == 0) {
      op = new CreateTable(command);
    } else if (command.find("drop table ") == 0) {
      op = new DropTable(command);
    } else if (command.find("create index ") == 0) {
      op = new CreateIndex(command);
    } else if (command.find("execfile ") == 0) {
      op = new Execfile(command);
    } else if (command.find("quit") == 0) {
      return COMMAND_QUIT;
    } else {
      throw string("Command `" + command + "` contains unknown operation.");
    }
  } catch (string& e) {
    cout << "Syntax Error: " << e << endl;
    return SYNTAX_ERROR;
  }
  try {
    return op->Execute();
  } catch (string& e) {
    cout << "Runtime Error: " << e << endl;
    return RUNTIME_ERROR;
  }
}
}  // namespace db
