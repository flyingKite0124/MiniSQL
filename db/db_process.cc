#include "db/db_process.h"

#include <stdexcept>
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
    } else if (command.find("drop index ") == 0) {
      op = new DropIndex(command);
    } else if (command.find("execfile ") == 0) {
      op = new Execfile(command);
    } else if (command.find("insert into ") == 0) {
      op = new InsertInto(command);
    } else if (command.find("select * from ") == 0) {
      op = new SelectFrom(command);
    } else if (command.find("delete from ") == 0) {
      op = new DeleteFrom(command);
    } else if (command.find("quit") == 0) {
      return COMMAND_QUIT;
    } else {
      throw invalid_argument("Command `" + command +
                             "` contains unknown operation.");
    }
  } catch (invalid_argument& e) {
    cout << "Syntax Error: " << e.what() << endl;
    return SYNTAX_ERROR;
  }
  try {
    return op->Execute();
  } catch (runtime_error& e) {
    cout << "Runtime Error: " << e.what() << endl;
    return RUNTIME_ERROR;
  }
}
}  // namespace db
