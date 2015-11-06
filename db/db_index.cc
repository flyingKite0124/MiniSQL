//
// Created by GreyTone on 15-11-4.
//

#include "db/db_index.h"

namespace db {

int CreateIndex(Table table, std::string attr_name) {
//    BufferDelegate.CreateIndex(table.GetName(), attr_name);
}

int DropIndex(Table table, std::string attr_name) {
//    BufferDelegate.DropIndex(table.GetName(), attr_name);
}

int RecreateIndex(Table table, std::string attr_name) {
    DropIndex(table, attr_name);
    CreateIndex(table, attr_name);
}

int InsertIndex(Table table, std::string attr_name, IndexPair pair) {
    std::string tableName = table.GetName();
//    int rootNumber = BufferDelegate.GetRootNumber(tableName, attr_name);
//    int height = BufferDelegate.GetHeightNumber(tableName, attr_name);
}

IndexPairList SelectIndex(Table table, std::string attr_name, Filter filter) {

}

int DeleteIndex(Table table, std::string attr_name, IndexPair pair) {

}

} // namespace db