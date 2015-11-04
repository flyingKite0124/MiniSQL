//
// Created by GreyStone on 15-11-4.
//

#ifndef DB_DB_INDEX_H
#define DB_DB_INDEX_H

#include <iostream>
#include <string>
#include <vector>

#include "db/db_global.h"
#include "db/db_type.h"
#include "db/db_buffer.h"

namespace  db {
    enum node_types {nleaf = 0, leaf = 1};
    struct BPT_node {
        node_types type;
        std::string keys[15];
        BPT_node *pointers[16];
    };

    bool CreateIndex(std::string table_name, std::string index_name);
    bool DropIndex(std::string table_name, std::string index_name);

    bool InsertIndexNode(std::string table_name, std::string index_name);
    bool RemoveIndexNode(std::string table_name, std::string index_name);
    bool Search(std::string table_name, std::string index_name);

    bool RecreateIndex(std::string table_name, std::string index_name);
}

#endif //DB_DB_INDEX_H
