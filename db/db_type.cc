#include "db/db_type.h"

#include <assert.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include "db/db_main.h"
#include "db/db_repl.h"
#include "base/string.h"

namespace db {

using namespace base;

inline RELATION StringToRelation(string s) {
  if (s == ">") return GT;
  if (s == "<") return LT;
  if (s == "<>") return NEQ;
  if (s == "=") return EQ;
  if (s == ">=") return GTE;
  if (s == "<=") return LTE;
  throw invalid_argument("`" + s + "` is not a valid comparer.");
}

Filter::Filter(string s) {
  istringstream in(s);
  string cur;
  in >> key;
  if (!String::IsWord(key)) {
    throw invalid_argument("`" + key + "` is not a valid attribute name");
  }
  in >> cur;
  op = StringToRelation(cur);
  in >> cur;
  value = String::LiteralToString(cur);
}

// Create Table class
// create table table_name (
//   attr type(size),
//   attr type(size),
//   ...
//   attr_type(attr),
//   attr_type(attr)
// );
CreateTableOperation::CreateTableOperation(string command) {
  op_type = TYPE_CREATE_TABLE;
  // Split string into header and definition body.
  size_t bracket_left = command.find('('),
         bracket_right = command.rfind(')');
  if (bracket_left == string::npos ||
      bracket_right == string::npos ||
      bracket_left > bracket_right)
    throw invalid_argument("Unmatched brackets in table definition.");
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
    throw invalid_argument("`" + table_name + "` is not a valid table name.");
  // Parse definitions to get attr_list
  int primaryKeyCounter = 0;
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
        throw invalid_argument("Attribute `" + attr_name + "` is not defined.");
      }
      primaryKeyCounter += 1;
    } else {
      Attribute attr;
      attr.attribute_type = TYPE_NONE;
      istringstream in(s);
      in >> attr.name;
      // Check if attribute_name is valid.
      if (!String::IsWord(attr.name)) {
        throw invalid_argument("`" + attr.name +
                               "` is not a valid atrribute name.");
      }
      string rest;
      size_t pos;
      getline(in, rest, '\0');
      rest = String::Trim(rest);
      if ((pos = rest.find('(')) != string::npos) {
        string type = String::Trim(rest.substr(0, pos));
        if (type != "char") {
          throw invalid_argument("Only the size of `char` can be specified.");
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
        } else {
          if (rest == "") {
            throw invalid_argument("Attribute `" + attr.name +
                                   "` needs a type.");
          } else {
            throw invalid_argument("Type `" + rest + "` is not a valid type.");
          }
        }
      }
      attr_list.push_back(attr);
    }
  }
  if (primaryKeyCounter == 0) {
    throw invalid_argument("No primary key is specified on table `" +
                            table_name + "`.");
  } else if (primaryKeyCounter > 1) {
    throw invalid_argument("Multiple primary keys are specified on table `" + 
                            table_name + "`.");
  }
  DEBUG << "Parsed: create table" << endl;
  DEBUG << "Table atrributes: " << endl;
  for (Attribute& attr: attr_list) {
    DEBUG << "+ Attribute name: " << attr.name << endl;
    DEBUG << "  Attribute Type: " << attr.type << endl;
    DEBUG << "  Attribute Size: " << attr.size << endl;
    DEBUG << "  Attribute MetaType: " << attr.attribute_type << endl;
  }
  DEBUG << "========================================================" << endl;
}
int CreateTableOperation::Execute() {
  throw runtime_error("Operation `create table` is not implemented.");
}

// Drop Table class
// drop table table_name;
DropTableOperation::DropTableOperation(string command) {
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
    throw invalid_argument("`" + cur + "` is not a valid table name.");
  }
  table_name = cur;
  DEBUG << "Parsed: drop table" << endl;
  DEBUG << "Table name: "<< table_name << endl;
  DEBUG << "==========================================================" << endl;
}
int DropTableOperation::Execute() {
  throw runtime_error("Operation `drop table` is not implemented.");
}

// Create Index class
// create index index_name on table_name(attr_name);
CreateIndexOperation::CreateIndexOperation(string command) {
  op_type = TYPE_CREATE_INDEX;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "create");
  in >> cur;
  assert(cur == "index");
  in >> index_name;
  if (!String::IsWord(index_name)) {
    throw invalid_argument("`" + index_name + "` is not a valid index name.");
  }
  in >> cur;
  if (cur != "on") {
    throw invalid_argument("Operation needs `on` keyword right after index name.");
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
int CreateIndexOperation::Execute() {
  throw runtime_error("Operation `create index` is not implemented.");
}

// Drop Index Class
// drop index index_name;
DropIndexOperation::DropIndexOperation(string command) {
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
    throw invalid_argument("`" + cur + "` is not a valid index name.");
  }
  index_name = cur;
  DEBUG << "Parsed: drop index" << endl;
  DEBUG << "Index name: "<< index_name << endl;
  DEBUG << "==========================================================" << endl;
}
int DropIndexOperation::Execute() {
  throw runtime_error("Operation `drop index` is not implemented.");
}

// Insert Into Class
// insert into table_name values (value 1, value 2, ..., value n);
InsertIntoOperation::InsertIntoOperation(string command) {
  op_type = TYPE_INSERT_INTO;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "insert");
  in >> cur;
  assert(cur == "into");
  in >> table_name;
  if (!String::IsWord(table_name)) {
    throw invalid_argument("`" + table_name + "` is not a valid table name.");
  }
  getline(in, cur, '\0');
  values = String::Split(String::Trim(String::TakeOffBracket(cur)), ',');
  for (string& s: values) {
    s = String::LiteralToString(String::Trim(s));
  }
  DEBUG << "Parsed: insert into" << endl;
  DEBUG << "Table name: " << table_name << endl;
  DEBUG << "Values: " << endl;
  for (string &s: values) {
    DEBUG << "+ Value: " << s << endl;
  }
  DEBUG << "==========================================================" << endl;
}
int InsertIntoOperation::Execute() {
  throw runtime_error("Operation `insert into` is not implemented.");
}

// Select From Class
// select * from table_name [where filter 1 and filter 2 and ... and filter n];
SelectFromOperation::SelectFromOperation(string command) {
  op_type = TYPE_SELECT_FROM;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "select");
  in >> cur;
  assert(cur == "*");
  in >> cur;
  assert(cur == "from");
  in >> table_name;
  if (!String::IsWord(table_name)) {
    throw invalid_argument("`" + table_name + "` is not a valid table name.");
  }
  // where
  if (in >> cur) {
    if (cur != "where") {
      throw invalid_argument("`where` is excepted before `" + cur + "`.");
    }
    getline(in, cur, '\0');
    cur = String::Trim(cur);
    auto _filters = String::Split(cur, "and");
    for (string filter: _filters) {
      filters.push_back(Filter(String::Trim(filter)));
    }
  }
  DEBUG << "Parsed: select from" << endl;
  DEBUG << "Table name: " << table_name << endl;
  DEBUG << "Filters: " << endl;
  for (Filter f: filters) {
    DEBUG << "+ Key: " << f.key
          << " Op: " << f.op
          << " Value: " << f.value << endl;
  }
  DEBUG << "==========================================================" << endl;
}
int SelectFromOperation::Execute() {
  throw runtime_error("Operation `select from` is not implemented.");
}

// Delete From Class
DeleteFromOperation::DeleteFromOperation(string command) {
  op_type = TYPE_DELETE_FROM;
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "delete");
  in >> cur;
  assert(cur == "from");
  in >> table_name;
  if (!String::IsWord(table_name)) {
    throw invalid_argument("`" + table_name + "` is not a valid table name.");
  }
  // where
  if (in >> cur) {
    if (cur != "where") {
      throw invalid_argument("`where` is excepted before `" + cur + "`.");
    }
    getline(in, cur, '\0');
    cur = String::Trim(cur);
    auto _filters = String::Split(cur, "and");
    for (string filter: _filters) {
      filters.push_back(Filter(String::Trim(filter)));
    }
  }
  DEBUG << "Parsed: delete from" << endl;
  DEBUG << "Table name: " << table_name << endl;
  DEBUG << "Filters: " << endl;
  for (Filter f: filters) {
    DEBUG << "+ Key: " << f.key
          << " Op: " << f.op
          << " Value: " << f.value << endl;
  }
  DEBUG << "==========================================================" << endl;
}
int DeleteFromOperation::Execute() {
  throw runtime_error("Operation `delete from` is not implemented.");
}

// Execfile Class
ExecfileOperation::ExecfileOperation(string command) {
  istringstream in(command);
  string cur;
  in >> cur;
  assert(cur == "execfile");
  getline(in, cur, '\0');
  filepath = String::Trim(cur);
}
int ExecfileOperation::Execute() {
  ifstream fin(filepath);
  if (!fin) {
    throw runtime_error("Fail to open file `" + filepath + "`.");
  }
  return db::DBREPL(true, fin);
}
}  // namespace db
