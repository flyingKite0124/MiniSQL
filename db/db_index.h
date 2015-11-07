//
// Created by GreyStone on 15-11-4.
//

#ifndef DB_DB_INDEX_H
#define DB_DB_INDEX_H

#include "db/db_global.h"
#include "db/db_type.h"
#include "db/db_buffer.h"
#include "db/db_record.h"

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

#define INT_FLOAT_ORDER 507
#define CHAR_STRI_ORDER 15
#define INT_FLOAT_ALL 4080
#define CHAR_STRI_ALL 3900

//typedef struct bplustree_int_node {
//    node_types node_type;
//    std::map<int, int> dptr;
//
//    int prev_ptr;
//    int next_ptr;
//} BPT_IntNode;

typedef struct bpt_int_node {
    bool isLeaf;
    int key[INT_FLOAT_ORDER];
    int pointer[INT_FLOAT_ORDER+1];
    int countKey;
} BPT_INT;

typedef struct bpt_char_node {
    bool isLeaf;
    std::string key[CHAR_STRI_ORDER];
    int pointer[CHAR_STRI_ORDER+1];
    int countKey;
} BPT_CHAR;

typedef struct bpt_float_node {
    bool isLeaf;
    float key[INT_FLOAT_ORDER];
    int pointer[INT_FLOAT_ORDER+1];
    int countKey;
} BPT_FLOAT;


class Index_Header {
public:
    std::string tableName;
    std::string attrName;
    Attribute attribute;
    int rootAddr;
//    int height;
//
//    std::stack<std::pair<int, char *>> indexPath;
//    std::map<int, BPT_IntNode *> indexParsedPathforInt;
//    std::map<int, BPT_CharNode *> indexParsedPathforChar;
//    std::map<int, BPT_FloatNode *> indexParsedPathforFloat;

    explicit Index_Header(Table table, std::string attr_name);
    virtual ~Index_Header();
};

void PrintIntIndex(Index_Header *index);

void _IndexReadBlock(Index_Header idx, BPT_INT *node, int pos);
void _IndexWriteBlock(Index_Header idx, BPT_INT *node, int pos);
void _IndexReadBlock(Index_Header idx, BPT_CHAR *node, int pos);
void _IndexWriteBlock(Index_Header idx, BPT_CHAR *node, int pos);
void _IndexReadBlock(Index_Header idx, BPT_FLOAT *node, int pos);
void _IndexWriteBlock(Index_Header idx, BPT_FLOAT *node, int pos);

int CreateIntIndex(Table table, std::string attr_name);
int CreateCharIndex(Table table, std::string attr_name);
int CreateFloatIndex(Table table, std::string attr_name);
int DropIndex(Table table, std::string attr_name);

int InsertIntIndex(Table table, std::string attr_name,IndexPair pair);
int InsertCharIndex(Table table, std::string attr_name,IndexPair pair);
int InsertFloatIndex(Table table, std::string attr_name,IndexPair pair);
int DeleteIntIndex(Table table, std::string attr_name,IndexPair pair);
int DeleteCharIndex(Table table, std::string attr_name,IndexPair pair);
int DeleteFloatIndex(Table table, std::string attr_name,IndexPair pair);
IndexPairList _Index_SelectIntNode(Table table, std::string attr_name, Filter filter);
IndexPairList _Index_SelectCharNode(Table table, std::string attr_name, Filter filter);
IndexPairList _Index_SelectFloatNode(Table table, std::string attr_name, Filter filter);

int RecreateIntIndex(Table table, std::string attr_name);
int RecreateCharIndex(Table table, std::string attr_name);
int RecreateFloatIndex(Table table, std::string attr_name);

} // namespace db

#endif //DB_DB_INDEX_H
