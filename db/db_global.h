//
// Created by greystone on 15-11-5.
//

#ifndef DB_DB_GLOBAL_H
#define DB_DB_GLOBAL_H

#include "db/db_type.h"
#include "db/db_buffer.h"

#include <vector>

struct Table {
    std::string table_name;
    std::vector<db::Attribute> attr_list;
public:
    db::Attribute GetAttribute(std::string attribute_name);
    std::vector<std::string> GetIndexes();
    std::vector<std::string> GetAllIndexes();
    std::vector<std::string> GetUnique();
    std::vector<std::string> GetAllUnique();
};

typedef std::vector<db::Filter> FilterList;

typedef std::pair<int, std::vector<std::string>> Tuple;
typedef std::vector<Tuple> TupleList;

typedef std::pair<int, std::string> IndexPair;
typedef std::vector<IndexPair> IndexPairList;


extern db::Buffer BufferDelegate;

#endif //DB_DB_GLOBAL_H
