#include "db/db_process.h"

#include <sys/time.h>
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
      op = new CreateTableOperation(command);
    } else if (command.find("drop table ") == 0) {
      op = new DropTableOperation(command);
    } else if (command.find("create index ") == 0) {
      op = new CreateIndexOperation(command);
    } else if (command.find("drop index ") == 0) {
      op = new DropIndexOperation(command);
    } else if (command.find("execfile ") == 0) {
      op = new ExecfileOperation(command);
    } else if (command.find("insert into ") == 0) {
      op = new InsertIntoOperation(command);
    } else if (command.find("select * from ") == 0) {
      op = new SelectFromOperation(command);
    } else if (command.find("delete from ") == 0) {
      op = new DeleteFromOperation(command);
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
    timeval start, end;
    // Benchmark
    gettimeofday(&start, NULL);
    int ret = op->Execute();
    gettimeofday(&end, NULL);
    DEBUG << "Process `" + command + "` OK! " <<
    1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000 <<
    " ms used." << endl;
    return ret;
  } catch (runtime_error& e) {
    cout << "Runtime Error: " << e.what() << endl;
    return RUNTIME_ERROR;
  }
}
}  // namespace db
