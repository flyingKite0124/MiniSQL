#include "db/db_catalog.h"

#include <iostream>
#include <stdexcept>
#include "db/db_main.h"
#include "db/db_global.h"

using namespace std;

namespace db {
namespace Catalog {

void CreateTable(Table table) {
  Table temp;
  // Table exists
  if (GetTable(table.GetName(), temp)) {
    throw runtime_error("Table `" + table.GetName() + "` already exists.");
  }
  DEBUG << table.GetName() << " " <<  table.GetPrimaryKey().name << endl;
  BufferDelegate.CreateTable(table.GetName(), table.GetPrimaryKey().name);
  DEBUG << "Buffer works" << endl;
}

bool GetTable(string table_name, Table& table) {
  if (BufferDelegate.CheckTable(table_name) == 0)
    return false;
  // TODO
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
