//
// Created by GreyStone on 15-11-4.
//

#ifndef DB_DB_INDEX_H
#define DB_DB_INDEX_H

#include "db/db_global.h"
#include "db/db_type.h"
#include "db/db_buffer.h"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>

#include <list>
#include <map>
#include <stack>
#include <vector>

namespace  db {

enum node_types {nonleaf = 0, leaf = 1};
// TODO: Move to db_type.h

typedef struct bplustree_int_node {
    node_types node_type;
    std::map<int, int> dptr;
    int prev_ptr;
    int next_ptr;
} BPT_IntNode;

typedef struct bplustree_char_node {
    node_types node_type;
    std::map<std::string, int> dptr;
    int next_ptr;
} BPT_CharNode;

typedef struct bplustree_float_node {
    node_types node_type;
    std::map<float, int> dptr;
    int next_ptr;
} BPT_FloatNode;

#define INT_FLOAT_ORDER 510
#define CHAR_STRI_ORDER 15
#define INT_FLOAT_ALL 4080
#define CHAR_STRI_ALL 3900

class Index_Header {
public:
    std::string tableName;
    std::string attrName;
    Attribute attribute;
    int rootAddr;
    int height;

    std::stack<std::pair<int, char *>> indexPath;
    std::map<int, BPT_IntNode *> indexParsedPathforInt;
    std::map<int, BPT_CharNode *> indexParsedPathforChar;
    std::map<int, BPT_FloatNode *> indexParsedPathforFloat;

    explicit Index_Header(Table table, std::string attr_name);
    virtual ~Index_Header();
};

//class Index_Header_Int {
//public:
//    explicit Index_Header_Int(Table table, std::string attr_name);
//    virtual ~Index_Header_Int();
//};

void PrintIntIndex(Index_Header *index);

int CreateIndex(Table table, std::string attr_name);
int DropIndex(Table table, std::string attr_name);

int InsertIndex(Table table, std::string attr_name,IndexPair pair);
IndexPairList SelectIndex(Table table, std::string attr_name,Filter filter);
int DeleteIndex(Table table, std::string attr_name,IndexPair pair);

int RecreateIndex(Table table, std::string attr_name);

} // namespace db

#endif //DB_DB_INDEX_H
