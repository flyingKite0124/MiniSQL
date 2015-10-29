#include "db/db_type.h"

#include <assert.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include "db/db_main.h"
#include "db/db_repl.h"
#include "base/string.h"

namespace db {
// Create Table class
// create table table_name (
//   attr type(size),
//   attr type(size),
//   ...
//   attr_type(attr),
//   attr_type(attr)
// );
using namespace base;
CreateTable::CreateTable(string command) {
  op_type = TYPE_CREATE_TABLE;
  // Split string into header and definition body.
  size_t bracket_left = command.find('('),
         bracket_right = command.rfind(')');
  if (bracket_left == string::npos ||
      bracket_right == string::npos ||
      bracket_left > bracket_right)
    throw string("Unmatched brackets in table definition.");
  string header = String::Trim(command.substr(0, bracket_left)),
    definition = String::Trim(command.substr(bracket_left + 1,
                                             bracket_right - bracket_left - 1));
  string cur;
  // Parse header to get table_name
  istringstream hin(header);
  hin >> cur;
  assert(cur == "create"); // create
  hin >> cur;
  assert(cur == "table"); // table
  getline(hin, cur, '\0');
  table_name = String::Trim(cur);
  // Check table_name is valid
  if (!String::IsWord(table_name))
    throw string("`" + table_name + "` is not a valid table name.");
  // Parse definitions to get attr_list
  vector<string> definitions = String::Split(definition, ',');
  for (string& s: definitions) {
    s = String::Trim(s);
    // Primary Key definition
    if (s.find("primary key") == 0) {
      string attr_name = String::Trim(String::TakeOffBracket(s.substr(11)));
      bool found = false;
      for (Attribute& attr: attr_list) {
        if (attr.name == attr_name) {
          attr.attribute_type = TYPE_PRIMARY_KEY;
          found = true;
          break;
        }
      }
      if (!found) {
        throw string("Attribute `" + attr_name + "` is not defined.");
      }
    } else {
      Attribute attr;
      attr.attribute_type = TYPE_NONE;
      istringstream in(s);
      in >> attr.name;
      // Check if attribute_name is valid.
      if (!String::IsWord(attr.name)) {
        throw string("`" + attr.name + "` is not a valid atrribute name.");
      }
      string rest;
      size_t pos;
      getline(in, rest, '\0');
      rest = String::Trim(rest);
      if ((pos = rest.find('(')) != string::npos) {
        string type = String::Trim(rest.substr(0, pos));
        if (type != "char") {
          throw string("Only the size of char type can be specified.");
        }
        attr.type = TYPE_CHAR;
        attr.size = String::ToInt(String::TakeOffBracket(rest.substr(pos)));
      } else {
        if (rest == "int") {
          attr.type = TYPE_INT;
          attr.size = 4;
        } else if (rest == "float") {
          attr.type = TYPE_FLOAT;
          attr.size = 4;
        } else
          throw string("Type `" + rest + "` is not a valid type.");
      }
      attr_list.push_back(attr);
    }
  }
  DEBUG << "Parsed: create table" << endl;
  for (Attribute& attr: attr_list) {
    DEBUG << "+ Attribute name: " << attr.name << endl;
    DEBUG << "  Attribute Type: " << attr.type << endl;
    DEBUG << "  Attribute Size: " << attr.size << endl;
    DEBUG << "  Attribute MetaType: " << attr.attribute_type << endl;
  }
  DEBUG << "========================================================" << endl;
}
int CreateTable::Execute() {
  throw string("Operation `create table` is not implemented.");
}
// Drop Table class
DropTable::DropTable(string command) {
  op_type = TYPE_DROP_TABLE;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "drop");
  in >> cur;
  assert(cur == "table");
  getline(in, cur, '\0');
  cur = String::Trim(cur.substr(0, cur.length()));
  if (!String::IsWord(cur)) {
    throw string("`" + cur + "` is not a valid table name.");
  }
  table_name = cur;
  DEBUG << "Parsed: drop table" << endl;
  DEBUG << "Table name: "<< table_name << endl;
  DEBUG << "==========================================================" << endl;
}
int DropTable::Execute() {
  throw string("Operation `drop table` is not implemented.");
}
// Create Index class
CreateIndex::CreateIndex(string command) {
  op_type = TYPE_CREATE_INDEX;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "create");
  in >> cur;
  assert(cur == "index");
  in >> index_name;
  if (!String::IsWord(index_name)) {
    throw string("`" + index_name + "` is not a valid index name.");
  }
  in >> cur;
  if (cur != "on") {
    throw string("Operation needs `on` keyword right after index name.");
  }
  getline(in, table_name, '(');
  table_name = String::Trim(table_name);
  getline(in, cur, '\0');
  cur = "(" + cur;
  attr_name = String::Trim(String::TakeOffBracket(cur));
  DEBUG << "Parsed: create index" << endl;
  DEBUG << "Index name: " << index_name << endl;
  DEBUG << "Table name: " << table_name << endl;
  DEBUG << "Attribute name: " << attr_name << endl;
  DEBUG << "==========================================================" << endl;
}
int CreateIndex::Execute() {
  throw string("Operation `create index` is not implemented.");
}
// Drop Index Class
DropIndex::DropIndex(string command) {
  op_type = TYPE_DROP_INDEX;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "drop");
  in >> cur;
  assert(cur == "index");
  getline(in, cur, '\0');
  cur = String::Trim(cur.substr(0, cur.length()));
  if (!String::IsWord(cur)) {
    throw string("`" + cur + "` is not a valid index name.");
  }
  index_name = cur;
  DEBUG << "Parsed: drop index" << endl;
  DEBUG << "Table name: "<< index_name << endl;
  DEBUG << "==========================================================" << endl;
}
int DropIndex::Execute() {
  throw string("Operation `drop index` is not implemented.");
}
// Insert Into Class
// Select From Class
// Delete From Class
// Execfile Class
Execfile::Execfile(string command) {
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "execfile");
  getline(in, cur, '\0');
  filepath = String::Trim(cur);
}
int Execfile::Execute() {
  ifstream fin(filepath);
  if (!fin) {
    throw string("Fail to open file `" + filepath + "`.");
  }
  return db::DBREPL(true, fin);
}
}  // namespace db
