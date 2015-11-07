//
// Created by GreyTone on 15-11-4.
//

#include "db/db_index.h"

namespace db {

Index_Header::Index_Header(Table table, std::string attr_name) {
    tableName = table.GetName();
    attrName = attr_name;
    std::vector<Attribute> attrList = table.GetAttributes();
    for(std::vector<Attribute>::iterator itAttrList = attrList.begin(); itAttrList != attrList.end(); itAttrList++) {
        if(attr_name.compare(itAttrList->name) == 0) {
            attribute = *itAttrList;
            break;
        }
    }
    rootAddr = BufferDelegate.GetRootNumber(tableName, attrName);
    height = BufferDelegate.GetHeightNumber(tableName, attrName);
}
Index_Header::~Index_Header() {
    while(!indexPath.empty()) {
        delete[] indexPath.top().second;
        indexPath.pop();
    }
    // TODO: Other Map Index
}

int CreateIndex(Table table, std::string attr_name) {
    std::string tableName = table.GetName();
    db::BufferDelegate.CreateIndex(tableName, attr_name);

    // Reset Root Nodes
//    int physicalAddr = BufferDelegate.GetRootNumber(tableName, attr_name);
//    char block[4096];
//    memset(block, 0x00, 4096);
//    block[0] = (char)leaf;
//    BufferDelegate.WriteIndexBlock(tableName, attr_name, physicalAddr, block);
//
//    Index_Header index(table, attr_name);
//    PrintIntIndex(&index);
    return 1;
}

int DropIndex(Table table, std::string attr_name) {
    BufferDelegate.DropIndex(table.GetName(), attr_name);
}

int RecreateIndex(Table table, std::string attr_name) {
    DropIndex(table, attr_name);
    CreateIndex(table, attr_name);
}

void _Index_ParseIntNode(char *block, BPT_IntNode *parsedBlock) {
    for(int i = 1; i < 4080; i += 12) {
        int data, ptr;
        memcpy(&data, block+i, sizeof(int));
        memcpy(&ptr, block+i+sizeof(int), sizeof(int));
        if(ptr == 0) continue;
        parsedBlock->dptr.insert(std::pair<int, int>(data, ptr));
    }
    int next_ptr;
    memcpy(&next_ptr, block+4080, sizeof(int));
    parsedBlock->next_ptr = next_ptr;
}
void _Index_ParseCharNode(char *block, BPT_CharNode *parsedBlock) {
    for(int i = 1; i < 3900; i += 260) {
        std::string data; char tmpdata[256]; int ptr;
        std::stringstream ss;
        memcpy(&tmpdata, block+i, 256);
        memcpy(&ptr, block+i+256, sizeof(int));
        if(ptr == 0) continue;
        ss << tmpdata;  ss >> data;
        parsedBlock->dptr.insert(std::pair<std::string, int>(data, ptr));
    }
    int next_ptr;
    memcpy(&next_ptr, block+3900, sizeof(int));
    parsedBlock->next_ptr = next_ptr;
}
void _Index_ParseFloatNode(char *block, BPT_FloatNode *parsedBlock) {
    for(int i = 1; i < 4080; i += 12) {
        float data; int ptr;
        memcpy(&data, block+i, sizeof(float));
        memcpy(&ptr, block+i+sizeof(float), sizeof(int));
        if(ptr == 0) continue;
        parsedBlock->dptr.insert(std::pair<float, int>(data, ptr));
    }
    int next_ptr;
    memcpy(&next_ptr, block+4080, sizeof(int));
    parsedBlock->next_ptr = next_ptr;
}

void PrintIntIndex(Index_Header *index) {
    std::cout << "Output All Info in " << index->tableName << "(";
    std::cout << index->attrName << ")" << std::endl;
    std::stack<std::pair<int, char *>> tmpIndexPath = index->indexPath;
    std::map<int, BPT_IntNode *>::iterator it;

    while(!tmpIndexPath.empty()) {
        int location = tmpIndexPath.top().first;
        std::cout << "\tON block(" << location << "):\n";
        std::cout << "\t\tRAW: {" << tmpIndexPath.top().second << "}\n";
        it = index->indexParsedPathforInt.find(location);
        std::cout << "\t\tParsed: {\n";
        BPT_IntNode *node = it->second;
        for(std::map<int, int>::iterator itor = node->dptr.begin(); itor != node->dptr.end(); itor++) {
            std::cout << "\t\t\tKey:" << itor->first <<"- block:" << itor->second << std::endl;
        }
        std::cout << "\t\t}\n";
        tmpIndexPath.pop();
    }
}

int InsertIndex(Table table, std::string attr_name, IndexPair pair) {
    // Get Information of Attribute
    Index_Header index(table, attr_name);

    node_types reachedLeafNode = nonleaf;
    int currentAddr = index.rootAddr;
    // Analyse IndexPath
    while(!reachedLeafNode) {
        char *block = new char[4096];
        BufferDelegate.ReadIndexBlock(index.tableName, attr_name, currentAddr, block);
        index.indexPath.push(std::make_pair(currentAddr, block));
        reachedLeafNode = (node_types)block[0];
        // Parsing Index Block
        switch (index.attribute.type) {
            case TYPE_INT: {
                BPT_IntNode *parsedBlock = new BPT_IntNode;
                parsedBlock->node_type = reachedLeafNode;
                _Index_ParseIntNode(block, parsedBlock);
                index.indexParsedPathforInt.insert(std::pair<int, BPT_IntNode*>(currentAddr, parsedBlock));
            } break;
            case TYPE_CHAR: {
                BPT_CharNode *parsedBlock = new BPT_CharNode;
                parsedBlock->node_type = reachedLeafNode;
                _Index_ParseCharNode(block, parsedBlock);
                index.indexParsedPathforChar.insert(std::pair<int, BPT_CharNode*>(currentAddr, parsedBlock));
            } break;
            case TYPE_FLOAT: {
                BPT_FloatNode *parsedBlock = new BPT_FloatNode;
                parsedBlock->node_type = reachedLeafNode;
                _Index_ParseFloatNode(block, parsedBlock);
                index.indexParsedPathforFloat.insert(std::pair<int, BPT_FloatNode*>(currentAddr, parsedBlock));
            } break;
        }

        //Get Next Address(Physical)
        std::stringstream ss;
        ss << pair.second;
        switch (index.attribute.type) {
            case TYPE_INT: {
                int key;    ss >> key;
                int location = index.indexPath.top().first;
                BPT_IntNode *parsedBlock = index.indexParsedPathforInt.find(location)->second;
                std::map<int, int>::iterator it = parsedBlock->dptr.find(key);
                if (it != parsedBlock->dptr.end()) {
                    if(it->second) currentAddr = it->second;
                    else currentAddr = 0;
                } else {
                    currentAddr = -5;
                    for(it = parsedBlock->dptr.begin(); it != parsedBlock->dptr.lower_bound(key); it++) {
                        if(it->second) currentAddr = it->second;
                    }
                    if(currentAddr == -5) currentAddr = 0;
                }
            } break;
            case TYPE_CHAR: {
                std::string key;    ss >> key;
                int location = index.indexPath.top().first;
                BPT_CharNode *parsedBlock = index.indexParsedPathforChar.find(location)->second;
                std::map<std::string, int>::iterator it = parsedBlock->dptr.find(key);
                if (it != parsedBlock->dptr.end()) {
                    if(it->second) currentAddr = it->second;
                    else currentAddr = 0;
                } else {
                    currentAddr = -5;
                    for(it = parsedBlock->dptr.begin(); it != parsedBlock->dptr.lower_bound(key); it++) {
                        if(it->second) currentAddr = it->second;
                    }
                    if(currentAddr == -5) currentAddr = 0;
                }
            } break;
            case TYPE_FLOAT: {
                float key;  ss >> key;
                int location = index.indexPath.top().first;
                BPT_FloatNode *parsedBlock = index.indexParsedPathforFloat.find(location)->second;
                std::map<float, int>::iterator it = parsedBlock->dptr.find(key);
                if (it != parsedBlock->dptr.end()) {
                    if(it->second) currentAddr = it->second;
                    else currentAddr = 0;
                } else {
                    currentAddr = -5;
                    for(it = parsedBlock->dptr.begin(); it != parsedBlock->dptr.lower_bound(key); it++) {
                        if(it->second) currentAddr = it->second;
                    }
                    if(currentAddr == -5) currentAddr = 0;
                }
            }
        }
    }


    // Overwrite Header of B+ Tree

    // Recycle Memory

    return 1;
}

IndexPairList SelectIndex(Table table, std::string attr_name, Filter filter) {
    // Get Header of B+ Tree
    std::string tableName = table.GetName();

}

int DeleteIndex(Table table, std::string attr_name, IndexPair pair) {

}

} // namespace db