#include "db/db_catalog.h"

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include "db/db_main.h"
#include "db/db_global.h"

using namespace std;

namespace db {
namespace Catalog {

void StringifyTable(Table table, char* chunk) {
  memset(chunk, 0, 4096);
  int idx = 0;
  auto attributes = table.GetAttributes();
  chunk[idx++] = attributes.size();
  for (auto& attr: attributes) {
    chunk[idx++] = attr.type;
    chunk[idx++] = attr.size;
    chunk[idx++] = attr.attribute_type;
    strcpy(chunk + idx, attr.name.c_str());
    idx += 64;
  }
}

Table ParseTable(string table_name, char* chunk) {
  int idx = 0;
  int attr_length = chunk[idx++];
  char tmp[128];
  memset(tmp, 0, sizeof(tmp));
  vector<Attribute> attr_list;
  for (int i = 0; i < attr_length; ++i) {
    Attribute attr;
    attr.type = (DB_DATA_TYPE)chunk[idx++];
    attr.size = (int)chunk[idx++];
    attr.attribute_type = (DB_ATTRIBUTE_TYPE)chunk[idx++];
    memcpy(tmp, chunk + idx, 64);
    idx += 64;
    attr.name = string(tmp);
    attr_list.push_back(attr);
  }
  Table table(table_name, attr_list);
  DEBUG << "Parsed: table structure from file system" << endl;
  DEBUG << "Table name: " << table.GetName() << endl;
  DEBUG << "Table atrributes: " << endl;
  for (Attribute& attr: table.GetAttributes()) {
    DEBUG << "+ Attribute name: " << attr.name << endl;
    DEBUG << "  Attribute Type: " << attr.type << endl;
    DEBUG << "  Attribute Size: " << attr.size << endl;
    DEBUG << "  Attribute MetaType: " << attr.attribute_type << endl;
  }
  DEBUG << "========================================================" << endl;
  return table;
}

void CreateTable(Table table) {
  Table temp;
  // Table exists
  if (GetTable(table.GetName(), temp)) {
    throw runtime_error("Table `" + table.GetName() + "` already exists.");
  }
  DEBUG << table.GetName() << " " <<  table.GetPrimaryKey().name << endl;
  BufferDelegate.CreateTable(table.GetName(), table.GetPrimaryKey().name);
  char chunk[4096];
  StringifyTable(table, chunk);
  BufferDelegate.WriteCatalogBlock(table.GetName(), chunk);
}

bool GetTable(string table_name, Table& table) {
  if (BufferDelegate.CheckTable(table_name) == 0)
    return false;
  char chunk[4096];
  BufferDelegate.ReadCatalogBlock(table_name, chunk);
  table = ParseTable(table_name, chunk);
  return true;
}

void DropTable(string table_name) {
  Table temp;
  // Table exists
  if (GetTable(table_name, temp)) {
    BufferDelegate.DropTable(table_name);
  } else {
    throw runtime_error("Table `" + table_name + "` is not found.");
  }
}

}  // namespace Catalog
}  // namespace db
