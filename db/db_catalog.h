#ifndef DB_DB_CATALOG_H
#define DB_DB_CATALOG_H

#include "db/db_type.h"

#include <string>

namespace db {
namespace Catalog {

void CreateTable(Table table);

bool GetTable(std::string table_name, Table& table);

void DropTable(std::string table_name);

}  // namespace Catalog
}  // namespace db

#endif  // DB_DB_CATALOG_H
