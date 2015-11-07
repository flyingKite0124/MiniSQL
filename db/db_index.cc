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
    // TODO: Other Map Index
}

int CreateIndex(Table table, std::string attr_name) {
    std::string tableName = table.GetName();
    db::BufferDelegate.CreateIndex(tableName, attr_name);

    // Reset Root Nodes
    int physicalAddr = BufferDelegate.GetRootNumber(tableName, attr_name);
    char block[4096];
    memset(block, 0x00, 4096);
    block[0] = (char)leaf;
    BufferDelegate.WriteIndexBlock(tableName, attr_name, physicalAddr, block);

//    Index_Header index(table, attr_name);
//    PrintIntIndex(&index);
    return 1;
}

int DropIndex(Table table, std::string attr_name) {
    BufferDelegate.DropIndex(table.GetName(), attr_name);
    return 1;
}

int RecreateIndex(Table table, std::string attr_name) {
    DropIndex(table, attr_name);
    CreateIndex(table, attr_name);
    return 1;
}

void _Index_ParseIntNode(char *block, BPT_IntNode *parsedBlock) {
    for(int i = 1; i < INT_FLOAT_ALL; i += 8) {
        int data, ptr;
        memcpy(&data, block+i, sizeof(int));
        memcpy(&ptr, block+i+sizeof(int), sizeof(int));
        if(ptr == 0) continue;
        parsedBlock->dptr.insert(std::pair<int, int>(data, ptr));
    }
    int prev_ptr;
    memcpy(&prev_ptr, block+INT_FLOAT_ALL, sizeof(int));
    parsedBlock->prev_ptr = prev_ptr;
    memcpy(&prev_ptr, block+INT_FLOAT_ALL+sizeof(int), sizeof(int));
    parsedBlock->next_ptr = prev_ptr;
}
void _Index_PackageIntNode(BPT_IntNode *parsedBlock, char *block) {
    block[0] = char(parsedBlock->node_type);
    int i = 1;
    for(std::map<int, int>::iterator it = parsedBlock->dptr.begin(); it != parsedBlock->dptr.end(); it++) {
        memcpy(block+i, &(it->first), sizeof(int));
        memcpy(block+i+sizeof(int), &(it->second), sizeof(int));
        i += 8;
    }
    memcpy(block+INT_FLOAT_ALL, &(parsedBlock->prev_ptr), sizeof(int));
    memcpy(block+INT_FLOAT_ALL+sizeof(int), &(parsedBlock->next_ptr), sizeof(int));
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
void _Index_PackageCharNode(BPT_CharNode *parsedBlock, char *block) {}
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
void _Index_PackageFloatNode(BPT_FloatNode *parsedBlock, char *block) {}

void PrintIntIndex(Index_Header *index) {
    std::cout << "==========PrintIntIndex==========\n";
    std::cout << "Output All Info in " << index->tableName << "(";
    std::cout << index->attrName << ")" << std::endl;
    std::stack<std::pair<int, char *>> tmpIndexPath = index->indexPath;
    std::map<int, BPT_IntNode *>::iterator it;

    while(!tmpIndexPath.empty()) {
        int location = tmpIndexPath.top().first;
        std::cout << "\tON block(" << location << "):\n";
        it = index->indexParsedPathforInt.find(location);
        std::cout << "\t\tParsed: {\n";
        BPT_IntNode *node = it->second;
        std::cout << "\t\t\tisLeaf:" << node->node_type << std::endl;
        for(std::map<int, int>::iterator itor = node->dptr.begin(); itor != node->dptr.end(); itor++) {
            std::cout << "\t\t\tKey:" << itor->first <<" - block:" << itor->second << std::endl;
        }
        std::cout << "\t\t}\n";
        tmpIndexPath.pop();
    }
    std::cout << "==========PrintIntIndex==========\n";
}

void _Index_UpdateNonleafNodeforInt(Index_Header index, int location, int key, bool isExtra, std::list<std::pair<int, char *>> *rec) {
    index.indexPath.pop();
    int parentLoc = index.indexPath.top().first;
    std::map<int, BPT_IntNode *>::iterator it = index.indexParsedPathforInt.find(parentLoc);
    BPT_IntNode *tarNode = it->second;

    if(!isExtra) {
        for(std::map<int, int>::iterator itor = tarNode->dptr.begin(); itor != tarNode->dptr.end(); itor++) {
            if(itor->second == location) {
                tarNode->dptr.erase(itor);
                break;
            }
        }
        tarNode->dptr.insert(std::pair<int, int>(key, location));
    } else {
        tarNode->prev_ptr = location;
    }
    char *block = new char[4096];
    _Index_PackageIntNode(tarNode, block);
    rec->push_back(std::pair<int, char*>(parentLoc, block));
}

void _Index_SplitNonleafNodeforInt(BPT_IntNode *front, BPT_IntNode *back) {
    BPT_IntNode tmpFront;
    int count = 0;
    for(std::map<int, int>::iterator it = front->dptr.begin(); it != front->dptr.end(); it++) {
        if(count < (INT_FLOAT_ORDER/2)) {
            tmpFront.dptr.insert(*it);
        }
        if(count == (INT_FLOAT_ORDER/2+1)) {
            back->prev_ptr = it->second;
        }
        if(count > (INT_FLOAT_ORDER/2+1)) {
            back->dptr.insert(*it);
        }
        count++;
    }
}

void _Index_InsertNonleafNodeforInt(Index_Header index, int height, int key, int value, std::list<std::pair<int, char *>> *rec) {
    if(!index.indexPath.empty()) {
        index.indexPath.pop();
        int nonLeafLoc = index.indexPath.top().first;
        std::map<int, BPT_IntNode *>::iterator it = index.indexParsedPathforInt.find(nonLeafLoc);
        BPT_IntNode *tarNode = it->second;
        if (tarNode->dptr.size() < INT_FLOAT_ORDER) {   // Not full
            if(key > tarNode->dptr.begin()->first) {   // Bigger than header
                tarNode->dptr.insert(std::pair<int, int>(key, value));
                char *block = new char[4096];
                _Index_PackageIntNode(tarNode, block);
                rec->push_back(std::pair<int, char*>(nonLeafLoc, block));
            } else {                                   // Become new header
                tarNode->dptr.insert(std::pair<int, int>(key, value));
                char *block = new char[4096];
                _Index_PackageIntNode(tarNode, block);
                rec->push_back(std::pair<int, char*>(nonLeafLoc, block));
                _Index_UpdateNonleafNodeforInt(index, nonLeafLoc, key, 0, rec);
            }
        } else {                            // Full
            int newLocation = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
            BPT_IntNode newNode;
            newNode.node_type = tarNode->node_type;
            tarNode->dptr.insert(std::pair<int, int>(key, value));
            _Index_SplitNonleafNodeforInt(tarNode, &newNode);

            char *block = new char[4096];
            _Index_PackageIntNode(&newNode, block);
            rec->push_back(std::pair<int, char*>(newLocation, block));
            block = new char[4096];
            _Index_PackageIntNode(tarNode, block);
            rec->push_back(std::pair<int, char*>(nonLeafLoc, block));
            _Index_UpdateNonleafNodeforInt(index, nonLeafLoc, key, 1, rec);
            _Index_InsertNonleafNodeforInt(index, height-1, newNode.dptr.begin()->first, newLocation, rec);
        }
    } else {                            // height == 0
        int nonLeafLoc = index.indexPath.top().first;
        std::map<int, BPT_IntNode *>::iterator it = index.indexParsedPathforInt.find(nonLeafLoc);
        BPT_IntNode *tarNode = it->second;
        int newLocation = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
        BPT_IntNode newNode;
        newNode.node_type = tarNode->node_type;
        tarNode->dptr.insert(std::pair<int, int>(key, value));
        _Index_SplitNonleafNodeforInt(tarNode, &newNode);

        int rootLocation = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
        BPT_IntNode rootNode;
        rootNode.node_type = nonleaf;
        rootNode.prev_ptr = nonLeafLoc;
        rootNode.dptr.insert(std::pair<int, int>(newNode.dptr.begin()->first, newLocation));
        BufferDelegate.SetRootNumber(index.tableName, index.tableName, rootLocation);
    }
}

void _Index_SplitLeafNodeforInt(BPT_IntNode *front, BPT_IntNode *back) {
    BPT_IntNode tmpFront;
    int count = 0;
    back->next_ptr = front->next_ptr;
    for(std::map<int, int>::iterator it = front->dptr.begin(); it != front->dptr.end(); it++) {
        if (count < (INT_FLOAT_ORDER / 2)) {
            tmpFront.dptr.insert(*it);
        }
        if (count == (INT_FLOAT_ORDER / 2)) {
            tmpFront.next_ptr = (it++)->second;
            it--;
        }
        if (count == (INT_FLOAT_ORDER / 2 + 1)) {
            back->prev_ptr = it->second;
        }
        if (count > (INT_FLOAT_ORDER / 2 + 1)) {
            back->dptr.insert(*it);
        }
        count++;
    }
    front->dptr.clear();
    front->dptr = tmpFront.dptr;
}

void _Index_InsertLeafNodeforInt(Index_Header *index, int location, int key, int value, std::list<std::pair<int, char *>> *rec) {
    BPT_IntNode *tarNode = index->indexParsedPathforInt.find(location)->second;
    if(tarNode->dptr.size() < INT_FLOAT_ORDER) {
        if(key > tarNode->dptr.begin()->first) {   // Bigger than header
            tarNode->dptr.insert(std::pair<int, int>(key, value));
            char *block = new char[4096];
            _Index_PackageIntNode(tarNode, block);
            rec->push_back(std::pair<int, char*>(location, block));
        } else {                                    // Become new header
            tarNode->dptr.insert(std::pair<int, int>(key, value));
            char *block = new char[4096];
            _Index_PackageIntNode(tarNode, block);
            rec->push_back(std::pair<int, char*>(location, block));
            _Index_UpdateNonleafNodeforInt(*index, location, key, 0, rec);
        }
    } else {    // Full
        if(key < tarNode->dptr.begin()->first) {
            int newLocation = BufferDelegate.GetEmptyIndexBlock(index->tableName, index->attrName);
            BPT_IntNode newNode;
            newNode.node_type = tarNode->node_type;
            newNode.dptr.insert(std::pair<int, int>(key, value));
            newNode.next_ptr = location;
            char *block = new char[4096];
            _Index_PackageIntNode(&newNode, block);
            rec->push_back(std::pair<int, char*>(newLocation, block));
            _Index_UpdateNonleafNodeforInt(*index, location, key, 1, rec);
            _Index_InsertNonleafNodeforInt(*index, index->height-1, tarNode->dptr.begin()->first, location, rec);
        } else {
            std::map<int, int>::iterator upperIt = tarNode->dptr.upper_bound(key);
            if(upperIt != tarNode->dptr.end()) {     // Smaller than end
                int newLocation = BufferDelegate.GetEmptyIndexBlock(index->tableName, index->attrName);
                BPT_IntNode newNode;
                newNode.node_type = tarNode->node_type;
                tarNode->dptr.insert(std::pair<int, int>(key, value));
                _Index_SplitLeafNodeforInt(tarNode, &newNode);

                char *block = new char[4096];
                _Index_PackageIntNode(&newNode, block);
                rec->push_back(std::pair<int, char*>(newLocation, block));
                block = new char[4096];
                _Index_PackageIntNode(tarNode, block);
                rec->push_back(std::pair<int, char*>(location, block));
                _Index_UpdateNonleafNodeforInt(*index, location, key, 1, rec);
                _Index_InsertNonleafNodeforInt(*index, index->height-1, newNode.dptr.begin()->first, newLocation, rec);
            } else {                                 // Bigger than end
                int newLocation = BufferDelegate.GetEmptyIndexBlock(index->tableName, index->attrName);
                BPT_IntNode newNode;
                newNode.node_type = tarNode->node_type;
                newNode.dptr.insert(std::pair<int, int>(key, value));
                newNode.next_ptr = 0;
                tarNode->next_ptr = newLocation;
                char *block = new char[4096];
                _Index_PackageIntNode(&newNode, block);
                rec->push_back(std::pair<int, char*>(newLocation, block));
                block = new char[4096];
                _Index_PackageIntNode(tarNode, block);
                rec->push_back(std::pair<int, char*>(location, block));
                _Index_UpdateNonleafNodeforInt(*index, location, key, 1, rec);
                _Index_InsertNonleafNodeforInt(*index, index->height-1, newNode.dptr.begin()->first, newLocation, rec);
            }
        }
    }
}



int InsertIndex(Table table, std::string attr_name, IndexPair pair) {
    // Get Information of Attribute
    Index_Header index(table, attr_name);
    node_types reachedLeafNode = nonleaf;
    int currentAddr = index.rootAddr;

    // InStack indexPath
    while (!reachedLeafNode) {
        char *block = new char[4096];
        BufferDelegate.ReadIndexBlock(index.tableName, attr_name, currentAddr, block);
        index.indexPath.push(std::make_pair(currentAddr, block));
        reachedLeafNode = (node_types) block[0];
        switch (index.attribute.type) {
            case TYPE_INT: {
                // Parsing Index Block
                BPT_IntNode *parsedBlock = new BPT_IntNode;
                parsedBlock->node_type = reachedLeafNode;
                _Index_ParseIntNode(block, parsedBlock);
                index.indexParsedPathforInt.insert(std::pair<int, BPT_IntNode *>(currentAddr, parsedBlock));
                // Find Lower Bound
                std::stringstream transKey;
                transKey << pair.second;
                int key;
                transKey >> key;
                std::map<int, int>::iterator it = parsedBlock->dptr.lower_bound(key);
                if (it != parsedBlock->dptr.end()) {    // Found lower bound
                    currentAddr = it->second;
                } else {
                    currentAddr = parsedBlock->prev_ptr;
                }
            }
                break;
            case TYPE_CHAR: {

            }
                break;
            case TYPE_FLOAT: {

            }
                break;
        }
        std::cout << "[DEBUG]<InsertIndex> currentAddr:" << currentAddr << std::endl;
    }


    // Insert Leaf Node
    std::stringstream transfer;
    transfer << pair.second;
    std::list<std::pair<int, char *>> overwritePath;
    switch (index.attribute.type) {
        case TYPE_INT: {
            int key;   transfer >> key;
            _Index_InsertLeafNodeforInt(&index, index.indexPath.top().first, key, pair.first, &overwritePath);
        }
            break;
    }

    PrintIntIndex(&index);

    // Overwrite Header of B+ Tree
    for(std::list<std::pair<int, char *>>::iterator it = overwritePath.begin(); it != overwritePath.end(); it++) {
        BufferDelegate.WriteIndexBlock(index.tableName, index.attrName, it->first, it->second);
    }
    // TODO: Maintainance Height

    // Recycle Memory
    for(std::list<std::pair<int, char *>>::iterator it = overwritePath.begin(); it != overwritePath.end(); it++) {
        delete[] it->second;
    }
    for(std::map<int, BPT_IntNode *>::iterator it = index.indexParsedPathforInt.begin(); it != index.indexParsedPathforInt.end(); it++) {
        delete it->second;
    }
    for(std::map<int, BPT_CharNode *>::iterator it = index.indexParsedPathforChar.begin(); it != index.indexParsedPathforChar.end(); it++) {
        delete it->second;
    }
    for(std::map<int, BPT_FloatNode *>::iterator it = index.indexParsedPathforFloat.begin(); it != index.indexParsedPathforFloat.end(); it++) {
        delete it->second;
    }

    return 1;
}

IndexPairList SelectIndex(Table table, std::string attr_name, Filter filter) {
    // Get Header of B+ Tree
    std::string tableName = table.GetName();
    IndexPairList shit;
    return shit;

}

int DeleteIndex(Table table, std::string attr_name, IndexPair pair) {
    // Get Information of Attribute
    Index_Header index(table, attr_name);
    node_types reachedLeafNode = nonleaf;
    int currentAddr = index.rootAddr;

    // InStack indexPath
    while (!reachedLeafNode) {
        char *block = new char[4096];
        BufferDelegate.ReadIndexBlock(index.tableName, attr_name, currentAddr, block);
        index.indexPath.push(std::make_pair(currentAddr, block));
        reachedLeafNode = (node_types) block[0];
        switch (index.attribute.type) {
            case TYPE_INT: {
                // Parsing Index Block
                BPT_IntNode *parsedBlock = new BPT_IntNode;
                parsedBlock->node_type = reachedLeafNode;
                _Index_ParseIntNode(block, parsedBlock);
                index.indexParsedPathforInt.insert(std::pair<int, BPT_IntNode *>(currentAddr, parsedBlock));
                // Find Lower Bound
                std::stringstream transKey;
                transKey << pair.second;
                int key;
                transKey >> key;
                std::map<int, int>::iterator it = parsedBlock->dptr.lower_bound(key);
                if (it != parsedBlock->dptr.end()) {    // Found lower bound
                    currentAddr = it->second;
                } else {
                    currentAddr = parsedBlock->prev_ptr;
                }
            }
                break;
            case TYPE_CHAR: {

            }
                break;
            case TYPE_FLOAT: {

            }
                break;
        }
    }

    return 1;
}

} // namespace db