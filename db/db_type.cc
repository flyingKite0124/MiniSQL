#include "db/db_type.h"

#include <assert.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
using namespace std;

#include "db/db_main.h"
#include "db/db_catalog.h"
#include "db/db_record.h"
#include "db/db_index.h"
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

inline int GetOutputSize(Attribute attr) {
  return max(attr.name.length() + 4,
             attr.type == TYPE_CHAR ? attr.size + 4 : 12);
}

Filter::Filter() {
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

Table::Table() {
}

Table::Table(string _table_name, vector<Attribute> _attr_list) {
  table_name = _table_name;
  attr_list = _attr_list;
}

string Table::GetName() {
  return table_name;
}

vector<Attribute> Table::GetAttributes() {
  return attr_list;
}

Attribute Table::GetPrimaryKey() {
  for (auto& attr: attr_list) {
    if (attr.attribute_type == TYPE_PRIMARY_KEY)
      return attr;
  }
  assert(false);
}

void Table::Indexify(string attr_name, string index_name) {
  for (auto& attr: attr_list)
    if (attr.index_name == index_name)
      throw runtime_error("Index `" + index_name + "` exists already.");
  for (auto& attr: attr_list) {
    if (attr.name == attr_name) {
      if (attr.index_name != "")
        throw runtime_error("An index has been created on attribute `" +
                            attr_name + "` of table `" + table_name + "`");
      if (attr.attribute_type == TYPE_NONE)
        throw runtime_error("An index must be created on unique attribute.");
      attr.index_name = index_name;
      DEBUG << "Before: Create Index" << endl;
      if (attr.type == TYPE_INT)
        CreateIntIndex(*this, attr_name);
      else if (attr.type == TYPE_FLOAT)
        CreateFloatIndex(*this, attr_name);
      else if (attr.type == TYPE_CHAR)
        CreateCharIndex(*this, attr_name);
      DEBUG << "After: Create Index" << endl;
      if (attr.attribute_type == TYPE_UNIQUE)
        attr.attribute_type = TYPE_INDEXED;
      break;
    }
  }
  Catalog::SaveTable(*this);
}

void Table::Unindexify(string index_name) {
  bool index_found = false;
  for (auto& attr: attr_list) {
    if (attr.index_name == index_name) {
      index_found = true;
      DropIndex(*this, attr.name);
      attr.attribute_type = TYPE_UNIQUE;
      Catalog::SaveTable(*this);
    }
  }
  if (!index_found)
    throw runtime_error("Index `" + index_name + "` is not found.");
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
  string table_name = String::Trim(cur);
  // Check table_name is valid
  if (!String::IsWord(table_name))
    throw invalid_argument("`" + table_name + "` is not a valid table name.");
  // Parse definitions to get attr_list
  vector<Attribute> attr_list;
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
      size_t pos, rpos;
      getline(in, rest, '\0');
      rest = String::Trim(rest);
      if ((pos = rest.find('(')) != string::npos &&
          (rpos = rest.find(')')) != string::npos) {
        string type = String::Trim(rest.substr(0, pos));
        if (type != "char") {
          throw invalid_argument("Only the size of `char` can be specified.");
        }
        attr.type = TYPE_CHAR;
        attr.size = String::ToInt(String::TakeOffBracket(rest.substr(pos, rpos - pos + 1)));
        if (rpos + 1 < rest.length())
          rest = rest.substr(rpos + 1);
        else
          rest = "";
      } else {
        istringstream rin(rest);
        string rcur;
        rin >> rcur;
        if (rcur == "int") {
          attr.type = TYPE_INT;
          attr.size = 4;
        } else if (rcur == "float") {
          attr.type = TYPE_FLOAT;
          attr.size = 4;
        } else {
          throw invalid_argument("Attribute `" + attr.name + "` needs a type.");
        }
        rin >> rest;
      }
      if (String::Trim(rest) == "unique")
        attr.attribute_type = TYPE_UNIQUE;
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
  table = Table(table_name, attr_list);
  DEBUG << "Parsed: create table" << endl;
  DEBUG << "Table name: " << table.GetName() << endl;
  DEBUG << "Table atrributes: " << endl;
  for (Attribute& attr: table.GetAttributes()) {
    DEBUG << "+ Attribute name: " << attr.name << endl;
    DEBUG << "  Attribute Type: " << attr.type << endl;
    DEBUG << "  Attribute Size: " << attr.size << endl;
    DEBUG << "  Attribute MetaType: " << attr.attribute_type << endl;
  }
  DEBUG << "========================================================" << endl;
}
int CreateTableOperation::Execute() {
  // DONE.
  // Create table
  Catalog::CreateTable(table);
  // Create primary key index
  auto primary_key = table.GetPrimaryKey();
  string index_name = "_pkidx_" + table.GetName();
  if (index_name.length() > 32)
    index_name = index_name.substr(0, 32);
#ifndef NOPRIMARYINDEX
#ifndef NOINDEX
  table.Indexify(primary_key.name, index_name);
#endif
#endif
  cout << "Create table `" << table.GetName() << "` successfully." << endl;
  return 0;
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
  // DONE.
  // Drop primary key index
  Table table;
  Catalog::GetTable(table_name, table);
#ifndef NOINDEX
  DropIndex(table, table.GetPrimaryKey().name);
#endif
  // Drop table
  Catalog::DropTable(table_name);
  cout << "Drop table `" << table_name << "` successfully." << endl;
  return 0;
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
  Table table;
  if (!Catalog::GetTable(table_name, table))
    throw runtime_error("Table `" + table_name + "` is not found.");
  bool found_attr = false;
  for (auto& attr: table.GetAttributes())
    if (attr.name == attr_name) {
      found_attr = true;
      break;
    }
  if (!found_attr)
    throw runtime_error("Attribute `" + attr_name + "` is not found on `" +
                        table_name + "`.");
#ifndef NOINDEX
  table.Indexify(attr_name, index_name);
  // XXX: HOTFIX
  ofstream fout("data/" + index_name + ".idxmap");
  fout << table_name << endl;
#else
  throw runtime_error("With mode NOINDEX, this is not allowed.");
#endif
  return 0;
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
#ifndef NOINDEX
  // XXX: HOTFIX
  string table_name;
  ifstream fin("data/" + index_name + ".idxmap");
  if (!fin.is_open())
    throw runtime_error("Index `" + index_name + "` is not found.");
  fin >> table_name;
  Table table;
  if (!Catalog::GetTable(table_name, table))
    throw runtime_error("Table `" + table_name + "` is not found.");
  table.Unindexify(index_name);
#else
  throw runtime_error("With mode NOINDEX, this is not allowed.");
#endif
  return 0;
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
  Table table;
  if (!Catalog::GetTable(table_name, table))
    throw runtime_error("Table `" + table_name + "` is not found.");
  auto attributes = table.GetAttributes();
  // Value size
  if (values.size() != attributes.size())
    throw runtime_error("The size of values provided is not matched to table.");
  // Unique test
  FilterList filters;
  int count = 0;
  for (auto& attribute: attributes) {
#ifndef NOINDEX
    // Not TYPE_NONE
    if (attribute.attribute_type == TYPE_UNIQUE) {
      filters.push_back(Filter(attribute.name + " = " + values[count]));
    } else if (attribute.attribute_type == TYPE_INDEXED ||
               attribute.attribute_type == TYPE_PRIMARY_KEY) {
      // With index
      Filter filter(attribute.name + " = " + values[count]);
      IndexPairList res;
      if (attribute.type == TYPE_INT)
        res = _Index_SelectIntNode(table, attribute.name, filter);
      else if (attribute.type == TYPE_FLOAT)
        res = _Index_SelectFloatNode(table, attribute.name, filter);
      else if (attribute.type == TYPE_CHAR)
        res = _Index_SelectCharNode(table, attribute.name, filter);
      cout << "Found " << res.size() << " tuples." << endl;
      for (auto& ip: res) {
        cout << "Block id: " << ip.first << ". Content: " << ip.second << endl;
      }
      if (res.size() > 0) {
        throw runtime_error("Unique constraints are not fulfilled.");
      }
    }
#else
    if (attribute.attribute_type >= TYPE_UNIQUE) {
      filters.push_back(Filter(attribute.name + " = " + values[count]));
    }
#endif
    ++count;
  }
  if (SelectRecordLinearOr(table, filters).size() > 0) {
    throw runtime_error("Unique constraints are not fulfilled.");
  }
  int block_id = InsertRecord(table, make_pair(-1, values));
#ifndef NOINDEX
  count = 0;
  for (auto& attribute: attributes) {
    if (attribute.attribute_type == TYPE_INDEXED ||
        attribute.attribute_type == TYPE_PRIMARY_KEY) {
      IndexPair ip = make_pair(block_id, values[count]);
      if (attribute.type == TYPE_INT)
        InsertIntIndex(table, attribute.name, ip);
      else if (attribute.type == TYPE_FLOAT)
        InsertFloatIndex(table, attribute.name, ip);
      else if (attribute.type == TYPE_CHAR)
        InsertCharIndex(table, attribute.name, ip);
    }
    ++count;
  }
#endif
#ifndef NOINSERTLOG
  cout << "Insert 1 record successfully." << endl;
#endif
  return 0;
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
  Table table;
  int counter = 0;
  if (!Catalog::GetTable(table_name, table))
    throw runtime_error("Table `" + table_name + "` is not found.");
  auto attributes = table.GetAttributes();
  TupleList tuples;
#ifndef NOINDEX
  // Check index
  int m_weight = 0;
  Attribute m_attr;
  Filter m_filter;
  for (auto& filter: filters) {
    // Indexed and is not !=
    if (filter.op == NEQ) continue;
    for (auto& attr: attributes) {
      if (filter.key != attr.name) continue;
      if (attr.attribute_type > m_weight) {
        m_weight = attr.attribute_type;
        m_attr = attr;
        m_filter = filter;
      }
    }
  }
  if (m_weight >= TYPE_INDEXED) {
    // With index
    IndexPairList ipl;
    if (m_attr.type == TYPE_INT)
      ipl = _Index_SelectIntNode(table, m_attr.name, m_filter);
    else if (m_attr.type == TYPE_FLOAT)
      ipl = _Index_SelectFloatNode(table, m_attr.name, m_filter);
    else if (m_attr.type == TYPE_CHAR)
      ipl = _Index_SelectCharNode(table, m_attr.name, m_filter);
    tuples = SelectRecordByList(table, m_attr.name, ipl, filters);
  } else {
    // Without index
    tuples = SelectRecordLinear(table, filters);
  }
#else
  tuples = SelectRecordLinear(table, filters);
#endif
  int columns = attributes.size();
  counter = 0;
  for (auto& attr: attributes) {
    cout << setw(GetOutputSize(attr)) << attr.name;
  }
  cout << endl;
  for (auto& tuple: tuples) {
    counter = 0;
    for (auto& value: tuple.second) {
      cout << setw(GetOutputSize(attributes[counter])) << value;
      ++counter;
    }
    cout << endl;
  }
  cout << "Query OK! " << tuples.size() << " records found." << endl;
  return 0;
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
  if (filters.size() > 1) {
    throw invalid_argument("More than one filters are not allowed.");
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
  Table table;
  if (!Catalog::GetTable(table_name, table))
    throw runtime_error("Table `" + table_name + "` is not found.");
  if (filters.size() == 0) {
    DeleteRecordAll(table);
#ifndef NOINDEX
    for (auto& attr: table.GetAttributes()) {
      if (attr.attribute_type >= TYPE_INDEXED) {
        if (attr.type == TYPE_INT)
          RecreateIntIndex(table, attr.name);
        else if (attr.type == TYPE_FLOAT)
          RecreateFloatIndex(table, attr.name);
        else
          RecreateCharIndex(table, attr.name);
      }
    }
#endif
    return 0;
  }
  Filter filter = filters[0];
  TupleList tuples;
  bool deleted = false;
#ifndef NOINDEX
  if (filter.op != NEQ) {
    for (auto& attr: table.GetAttributes()) {
      if (attr.name != filter.key) continue;
      if (attr.attribute_type >= TYPE_INDEXED) {
        IndexPairList ipl;
        if (attr.type == TYPE_INT)
          ipl = _Index_SelectIntNode(table, attr.name, filter);
        else if (attr.type == TYPE_FLOAT)
          ipl = _Index_SelectFloatNode(table, attr.name, filter);
        else if (attr.type == TYPE_CHAR)
          ipl = _Index_SelectCharNode(table, attr.name, filter);
        tuples = DeleteRecordByList(table, attr.name, ipl);
        deleted = true;
        break;
      }
    }
  }
#endif
  if (!deleted) {
    tuples = DeleteRecordLinear(table, filter);
#ifndef NOINDEX
  int counter = 0;
  for (auto& attr: table.GetAttributes()) {
    if (attr.attribute_type >= TYPE_INDEXED) {
      if (attr.type == TYPE_INT) {
        for (auto& tuple: tuples)
          DeleteIntIndex(table, attr.name, make_pair(tuple.first, tuple.second[counter]));
      } else if (attr.type == TYPE_FLOAT) {
        for (auto& tuple: tuples)
          DeleteFloatIndex(table, attr.name, make_pair(tuple.first, tuple.second[counter]));
      } else if (attr.type == TYPE_CHAR) {
        for (auto& tuple: tuples)
          DeleteCharIndex(table, attr.name, make_pair(tuple.first, tuple.second[counter]));
      }
    }
    ++counter;
  }
#endif
  }
  cout << "Delete OK! " << tuples.size() << " records deleted." << endl;
  return 0;
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
  // DONE.
  ifstream fin(filepath);
  if (!fin) {
    throw runtime_error("Fail to open file `" + filepath + "`.");
  }
  return db::DBREPL(true, fin);
}
}  // namespace db
