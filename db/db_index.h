//
// Created by GreyStone on 15-11-4.
//

#ifndef DB_DB_INDEX_H
#define DB_DB_INDEX_H

#include "db/db_global.h"
#include "db/db_type.h"
#include "db/db_buffer.h"

#include <iostream>
#include <string>
#include <vector>

namespace  db {

enum node_types {nleaf = 0, leaf = 1};
struct BPT_node {
    node_types type;
    std::string keys[15];
    BPT_node *pointers[16];
};

int CreateIndex(Table table, std::string attr_name);
int DropIndex(Table table, std::string attr_name);

int InsertIndex(Table table, std::string attr_name,IndexPair pair);
IndexPairList SelectIndex(Table table, std::string attr_name,Filter filter);
int DeleteIndex(Table table, std::string attr_name,IndexPair pair);

int RecreateIndex(Table table, std::string attr_name);

} // namespace db

#endif //DB_DB_INDEX_H
