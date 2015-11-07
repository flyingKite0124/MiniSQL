#include "db/db_main.h"

#include "base/io.h"
#include "base/command_line.h"
#include "db/db_help.h"
#include "db/db_repl.h"
#include "db_index.h"
#include "db_type.h"

#include <iostream>
#include <fstream>
using namespace std;

int db::debug = 0;

int main(int argc, const char* argv[]) {
//  cout << sizeof(char) << endl;
//  cout << sizeof(char *) << endl;
//  cout << sizeof(int) << endl;
//  cout << sizeof(int64_t) << endl;
//  cout << sizeof(float) << endl;
//  cout << sizeof(db::BPT_IntNode *) << endl;


  // GreysTone: Index Test
  db::Attribute attr1;
  attr1.name = "id";
  attr1.type = db::TYPE_INT;
  attr1.size = 4;
  attr1.attribute_type = db::TYPE_INDEXED;
  db::Attribute attr2;
  attr2.name = "id";
  attr2.type = db::TYPE_FLOAT;
  attr2.size = 4;
  attr2.attribute_type = db::TYPE_INDEXED;
  db::Attribute attr3;
  attr3.name = "id";
  attr3.type = db::TYPE_CHAR;
  attr3.size = 255;
  attr3.attribute_type = db::TYPE_INDEXED;
  std::vector<db::Attribute> attrs;
  attrs.push_back(attr1);
  attrs.push_back(attr2);
  attrs.push_back(attr3);
  db::Table indexTestTable("testTable", attrs);
  db::CreateIndex(indexTestTable, "id");

  // TODO: GreysTone[Remove]

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
