//
// Created by GreyTone on 15-11-4.
//

#include "db/db_index.h"

using namespace std;

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
}
Index_Header::~Index_Header() {
    // TODO: Other Map Index
}

int CreateIntIndex(Table table, std::string attr_name) {
    Index_Header index(table, attr_name);

    // Reset Root Nodes
    BPT_INT R;
    R.isLeaf = true;
    memset(R.key, 0, sizeof(R.key));
    memset(R.pointer, 0, sizeof(R.pointer));
    R.countKey = 0;
    _IndexWriteBlock(index, &R, index.rootAddr);

    return 1;
}
int CreateCharIndex(Table table, std::string attr_name) {
    Index_Header index(table, attr_name);

    // Reset Root Nodes
    BPT_CHAR R;
    R.isLeaf = true;
    memset(R.key, 0, sizeof(R.key));
    memset(R.pointer, 0, sizeof(R.pointer));
    R.countKey = 0;
    _IndexWriteBlock(index, &R, index.rootAddr);

    return 1;
}
int CreateFloatIndex(Table table, std::string attr_name) {
    Index_Header index(table, attr_name);

    // Reset Root Nodes
    BPT_FLOAT R;
    R.isLeaf = true;
    memset(R.key, 0, sizeof(R.key));
    memset(R.pointer, 0, sizeof(R.pointer));
    R.countKey = 0;
    _IndexWriteBlock(index, &R, index.rootAddr);

    return 1;
}
int DropIndex(Table table, std::string attr_name) {
    BufferDelegate.DropIndex(table.GetName(), attr_name);
    return 1;
}

int RecreateIntIndex(Table table, std::string attr_name) {
    DropIndex(table, attr_name);
    CreateIntIndex(table, attr_name);
    return 1;
}
int RecreateCharIndex(Table table, std::string attr_name) {
    DropIndex(table, attr_name);
    CreateCharIndex(table, attr_name);
    return 1;
}
int RecreateFloatIndex(Table table, std::string attr_name) {
    DropIndex(table, attr_name);
    CreateFloatIndex(table, attr_name);
    return 1;
}

void _IndexWriteBlock(Index_Header idx, BPT_INT *node, int pos) {
    char *block = new char[4096];
    memcpy(block, (char*)node, sizeof(BPT_INT));
    BufferDelegate.WriteIndexBlock(idx.tableName, idx.attrName, pos, block);
}
void _IndexReadBlock(Index_Header idx, BPT_INT *node, int pos) {
    char *block = new char[4096];
    BufferDelegate.ReadIndexBlock(idx.tableName, idx.attrName, pos, block);
    memcpy((char*)node, block, sizeof(BPT_INT));
}
void _IndexWriteBlock(Index_Header idx, BPT_CHAR *node, int pos) {
    char *block = new char[4096];
    memcpy(block, (char*)node, sizeof(BPT_CHAR));
    BufferDelegate.WriteIndexBlock(idx.tableName, idx.attrName, pos, block);
}
void _IndexReadBlock(Index_Header idx, BPT_CHAR *node, int pos) {
    char *block = new char[4096];
    BufferDelegate.ReadIndexBlock(idx.tableName, idx.attrName, pos, block);
    memcpy((char*)node, block, sizeof(BPT_CHAR));
}
void _IndexWriteBlock(Index_Header idx, BPT_FLOAT *node, int pos) {
    char *block = new char[4096];
    memcpy(block, (char*)node, sizeof(BPT_FLOAT));
    BufferDelegate.WriteIndexBlock(idx.tableName, idx.attrName, pos, block);
}
void _IndexReadBlock(Index_Header idx, BPT_FLOAT *node, int pos) {
    char *block = new char[4096];
    BufferDelegate.ReadIndexBlock(idx.tableName, idx.attrName, pos, block);
    memcpy((char*)node, block, sizeof(BPT_FLOAT));
}

void _Index_PrintBPT(Index_Header idx) {
    BPT_INT R;
    int i;
    for(i = 0 ; i < R.countKey; i++) {
        cout << "Key:" << R.key[i] << " - Ptr:" << R.pointer[i] << endl;
    }
    cout << R.pointer[i] << endl << endl;
}

void _Index_SplitNode(Index_Header idx, BPT_INT &father, BPT_INT &current, const int childnum) {
    // Split Full BPlus TreeNode
    int half = INT_FLOAT_ORDER/2 ;

    int i ;
    for(i=father.countKey; i>childnum; i--) {
        father.key[i] = father.key[i-1] ;
        father.pointer[i+1] = father.pointer[i];
    }
    father.countKey++;

    BPT_INT t;

    int address = BufferDelegate.GetEmptyIndexBlock(idx.tableName, idx.attrName);

    father.key[childnum] = current.key[half] ;
    father.pointer[childnum+1] = address;

    for( i = half+1; i < INT_FLOAT_ORDER; i++ )
    {
        t.key[i-half-1] = current.key[i] ;
        t.pointer[i-half-1] = current.pointer[i];
    }

    t.countKey = INT_FLOAT_ORDER - half - 1;
    t.pointer[t.countKey] = current.pointer[INT_FLOAT_ORDER];

    t.isLeaf = current.isLeaf ;

    current.countKey = half ;

    if(current.isLeaf)
    {
        current.countKey++;
        t.pointer[INT_FLOAT_ORDER] = current.pointer[INT_FLOAT_ORDER];
        current.pointer[INT_FLOAT_ORDER] = address ;
    }

    _IndexWriteBlock(idx, &t, address);

}
void _Index_SplitNode(Index_Header idx, BPT_CHAR &father, BPT_CHAR &current, const int childnum) {
    // Split Full BPlus TreeNode
    int half = CHAR_STRI_ORDER/2 ;

    int i ;
    for(i=father.countKey; i>childnum; i--) {
        father.key[i] = father.key[i-1] ;
        father.pointer[i+1] = father.pointer[i];
    }
    father.countKey++;

    BPT_CHAR t;

    int address = BufferDelegate.GetEmptyIndexBlock(idx.tableName, idx.attrName);

    father.key[childnum] = current.key[half] ;
    father.pointer[childnum+1] = address;

    for( i = half+1; i < CHAR_STRI_ORDER; i++ )
    {
        t.key[i-half-1] = current.key[i] ;
        t.pointer[i-half-1] = current.pointer[i];
    }

    t.countKey = CHAR_STRI_ORDER - half - 1;
    t.pointer[t.countKey] = current.pointer[CHAR_STRI_ORDER];

    t.isLeaf = current.isLeaf ;

    current.countKey = half ;

    if(current.isLeaf)
    {
        current.countKey++;
        t.pointer[CHAR_STRI_ORDER] = current.pointer[CHAR_STRI_ORDER];
        current.pointer[CHAR_STRI_ORDER] = address ;
    }

    _IndexWriteBlock(idx, &t, address);

}
void _Index_SplitNode(Index_Header idx, BPT_FLOAT &father, BPT_FLOAT &current, const int childnum) {
    // Split Full BPlus TreeNode
    int half = INT_FLOAT_ORDER/2 ;

    int i ;
    for(i=father.countKey; i>childnum; i--) {
        father.key[i] = father.key[i-1] ;
        father.pointer[i+1] = father.pointer[i];
    }
    father.countKey++;

    BPT_FLOAT t;

    int address = BufferDelegate.GetEmptyIndexBlock(idx.tableName, idx.attrName);

    father.key[childnum] = current.key[half] ;
    father.pointer[childnum+1] = address;

    for( i = half+1; i < INT_FLOAT_ORDER; i++ )
    {
        t.key[i-half-1] = current.key[i] ;
        t.pointer[i-half-1] = current.pointer[i];
    }

    t.countKey = INT_FLOAT_ORDER - half - 1;
    t.pointer[t.countKey] = current.pointer[INT_FLOAT_ORDER];

    t.isLeaf = current.isLeaf ;

    current.countKey = half ;

    if(current.isLeaf)
    {
        current.countKey++;
        t.pointer[INT_FLOAT_ORDER] = current.pointer[INT_FLOAT_ORDER];
        current.pointer[INT_FLOAT_ORDER] = address ;
    }

    _IndexWriteBlock(idx, &t, address);

}

void _Insert_IndexforInt(Index_Header idx, int current, IndexPair pair) {
    BPT_INT node;
    _IndexReadBlock(idx, &node, current);

    int	i, pairKey = __StringToInt(pair.second);
    for(i=0 ; i<node.countKey && node.key[i]<pairKey; i++);

    if(i<node.countKey && node.isLeaf && node.key[i]==pairKey) {
        if(node.pointer[i] == -1) node.pointer[i] = pair.first;
        else return ;
    }

    if(!node.isLeaf)
    {
        BPT_INT back;
        _IndexReadBlock(idx, &back, node.pointer[i]);

        if(back.countKey == INT_FLOAT_ORDER) {
            // If sub node of back is full, then split
            _Index_SplitNode(idx, node, back, i);
            _IndexWriteBlock(idx, &node, current);
            _IndexWriteBlock(idx, &back, node.pointer[i]);
        }
        if(pairKey<=node.key[i] || i==node.countKey) {
            _Insert_IndexforInt(idx, node.pointer[i], pair);
        } else {
            _Insert_IndexforInt(idx, node.pointer[i+1], pair);
        }

    }
    else {			//If is Leaf, insert directly
        for(int j = node.countKey ; j > i ; j--) {
            node.key[j] = node.key[j-1] ;
            node.pointer[j] = node.pointer[j-1] ;
        }
        node.key[i] = pairKey ;
        node.countKey++;
        node.pointer[i] = pair.first;

        _IndexWriteBlock(idx, &node, current);
    }
}
void _Insert_IndexforChar(Index_Header idx, int current, IndexPair pair) {
    BPT_CHAR node;
    _IndexReadBlock(idx, &node, current);

    int	i;
    std::string pairKey = pair.second;
    for(i=0 ; i<node.countKey && node.key[i]<pairKey; i++);

    if(i<node.countKey && node.isLeaf && node.key[i]==pairKey) {
        if(node.pointer[i] == -1) node.pointer[i] = pair.first;
        else return ;
    }

    if(!node.isLeaf)
    {
        BPT_CHAR back;
        _IndexReadBlock(idx, &back, node.pointer[i]);

        if(back.countKey == CHAR_STRI_ORDER) {
            _Index_SplitNode(idx, node, back, i);
            _IndexWriteBlock(idx, &node, current);
            _IndexWriteBlock(idx, &back, node.pointer[i]);
        }
        if(pairKey<=node.key[i] || i==node.countKey) {
            _Insert_IndexforChar(idx, node.pointer[i], pair);
        } else {
            _Insert_IndexforChar(idx, node.pointer[i+1], pair);
        }

    }
    else {
        for(int j = node.countKey ; j > i ; j--) {
            node.key[j] = node.key[j-1] ;
            node.pointer[j] = node.pointer[j-1] ;
        }
        node.key[i] = pairKey ;
        node.countKey++;
        node.pointer[i] = pair.first;

        _IndexWriteBlock(idx, &node, current);
    }
}
void _Insert_IndexforFloat(Index_Header idx, int current, IndexPair pair) {
    BPT_FLOAT node;
    _IndexReadBlock(idx, &node, current);

    int	i;
    float pairKey = __StringToFloat(pair.second);
    for(i=0 ; i<node.countKey && node.key[i]<pairKey; i++);

    if(i<node.countKey && node.isLeaf && node.key[i]==pairKey) {
        if(node.pointer[i] == -1) node.pointer[i] = pair.first;
        else return ;
    }

    if(!node.isLeaf)	//如果不是叶节点
    {
        BPT_FLOAT back;
        _IndexReadBlock(idx, &back, node.pointer[i]);

        if(back.countKey == INT_FLOAT_ORDER) {
            _Index_SplitNode(idx, node, back, i);
            _IndexWriteBlock(idx, &node, current);
            _IndexWriteBlock(idx, &back, node.pointer[i]);
        }
        if(pairKey<=node.key[i] || i==node.countKey) {
            _Insert_IndexforFloat(idx, node.pointer[i], pair);
        } else {
            _Insert_IndexforFloat(idx, node.pointer[i+1], pair);
        }

    }
    else {
        for(int j = node.countKey ; j > i ; j--) {
            node.key[j] = node.key[j-1] ;
            node.pointer[j] = node.pointer[j-1] ;
        }
        node.key[i] = pairKey ;
        node.countKey++;
        node.pointer[i] = pair.first;

        _IndexWriteBlock(idx, &node, current);
    }
}
int InsertIntIndex(Table table, std::string attr_name,IndexPair pair) {
    Index_Header index(table, attr_name);

    BPT_INT node;
    _IndexReadBlock(index, &node, index.rootAddr);

    if(node.countKey == INT_FLOAT_ORDER) {
        BPT_INT nR;
        nR.isLeaf = false;
        nR.countKey = 0;
        nR.pointer[0] = index.rootAddr;

        _Index_SplitNode(index, nR, node, 0);
        _IndexWriteBlock(index, &node, index.rootAddr);

        index.rootAddr = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
        _IndexWriteBlock(index, &nR, index.rootAddr);
        BufferDelegate.SetRootNumber(index.tableName, index.attrName, index.rootAddr);
    }
    _Insert_IndexforInt(index, index.rootAddr, pair);

    return 1;
}
int InsertCharIndex(Table table, std::string attr_name,IndexPair pair) {
    Index_Header index(table, attr_name);

    BPT_CHAR node;
    _IndexReadBlock(index, &node, index.rootAddr);

    if(node.countKey == CHAR_STRI_ORDER) {
        BPT_CHAR nR;
        nR.isLeaf = false;
        nR.countKey = 0;
        nR.pointer[0] = index.rootAddr;

        _Index_SplitNode(index, nR, node, 0);
        _IndexWriteBlock(index, &node, index.rootAddr);

        index.rootAddr = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
        _IndexWriteBlock(index, &nR, index.rootAddr);
        BufferDelegate.SetRootNumber(index.tableName, index.attrName, index.rootAddr);
    }
    _Insert_IndexforChar(index, index.rootAddr, pair);

    return 1;
}
int InsertFloatIndex(Table table, std::string attr_name,IndexPair pair) {
    Index_Header index(table, attr_name);

    BPT_FLOAT node;
    _IndexReadBlock(index, &node, index.rootAddr);

    if(node.countKey == INT_FLOAT_ORDER) {
        BPT_FLOAT nR;
        nR.isLeaf = false;
        nR.countKey = 0;
        nR.pointer[0] = index.rootAddr;

        _Index_SplitNode(index, nR, node, 0);
        _IndexWriteBlock(index, &node, index.rootAddr);

        index.rootAddr = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
        _IndexWriteBlock(index, &nR, index.rootAddr);
        BufferDelegate.SetRootNumber(index.tableName, index.attrName, index.rootAddr);
    }
    _Insert_IndexforFloat(index, index.rootAddr, pair);

    return 1;
}

IndexPair _Index_SelectBoarderIntNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPair result;

    int i;  BPT_INT a;
    int current = index.rootAddr;
    do
    {
        _IndexReadBlock(index, &a, current);
        int pairKey = __StringToInt(filter.value);
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0) {
            result = std::pair<int, std::string>(a.pointer[i], __IntToString(a.key[i]));
//            cout << a.key[i] << "-" << a.pointer[i] << endl;
            return result;
        }
        current = a.pointer[i] ;

    }while(!a.isLeaf);
    return result;
}
IndexPair _Index_SelectBoarderCharNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPair result;

    int i;  BPT_CHAR a;
    int current = index.rootAddr;
    do
    {
        _IndexReadBlock(index, &a, current);
        std::string pairKey = filter.value;
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0) {
            result = std::pair<int, std::string>(a.pointer[i], a.key[i]);
//            cout << a.key[i] << "-" << a.pointer[i] << endl;
            return result;
        }
        current = a.pointer[i] ;

    }while(!a.isLeaf);
    return result;
}
IndexPair _Index_SelectBoarderFloatNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPair result;

    int i;  BPT_FLOAT a;
    int current = index.rootAddr;
    do
    {
        _IndexReadBlock(index, &a, current);
        float pairKey = __StringToFloat(filter.value);
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0) {
            result = std::pair<int, std::string>(a.pointer[i], __FloatToString(a.key[i]));
//            cout << a.key[i] << "-" << a.pointer[i] << endl;
            return result;
        }
        current = a.pointer[i] ;

    }while(!a.isLeaf);
    return result;
}
IndexPairList _Index_SelectIntNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPairList result;

    switch (filter.op) {
        case EQ: {
            IndexPair limit = _Index_SelectBoarderIntNode(table, attr_name, filter);
            result.push_back(limit);
        } break;
        case NEQ: {
            BPT_INT head;
            IndexPair limit = _Index_SelectBoarderIntNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]));
                    if(line.second == limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case LT: {
            BPT_INT head;
            IndexPair limit = _Index_SelectBoarderIntNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]));
                    if(line.second < limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case GT: {
            BPT_INT head;
            IndexPair limit = _Index_SelectBoarderIntNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]));
                    if(line.second < limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case LTE: {
            BPT_INT head;
            IndexPair limit = _Index_SelectBoarderIntNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]));
                    if(line.second <= limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case GTE: {
            BPT_INT head;
            IndexPair limit = _Index_SelectBoarderIntNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]));
                    if(line.second <= limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        }
        return result;
    }
    return  result;
}
IndexPairList _Index_SelectCharNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPairList result;

    switch (filter.op) {
        case EQ: {
            IndexPair limit = _Index_SelectBoarderCharNode(table, attr_name, filter);
            result.push_back(limit);
        } break;
        case NEQ: {
            BPT_CHAR head;
            IndexPair limit = _Index_SelectBoarderCharNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], head.key[i]);
                    if(line.second == limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[CHAR_STRI_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[CHAR_STRI_ORDER]);
            }
        } break;
        case LT: {
            BPT_CHAR head;
            IndexPair limit = _Index_SelectBoarderCharNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], head.key[i]);
                    if(line.second < limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[CHAR_STRI_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[CHAR_STRI_ORDER]);
            }
        } break;
        case GT: {
            BPT_CHAR head;
            IndexPair limit = _Index_SelectBoarderCharNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], head.key[i]);
                    if(line.second < limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[CHAR_STRI_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[CHAR_STRI_ORDER]);
            }
        } break;
        case LTE: {
            BPT_CHAR head;
            IndexPair limit = _Index_SelectBoarderCharNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], head.key[i]);
                    if(line.second <= limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[CHAR_STRI_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[CHAR_STRI_ORDER]);
            }
        } break;
        case GTE: {
            BPT_CHAR head;
            IndexPair limit = _Index_SelectBoarderCharNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], head.key[i]);
                    if(line.second <= limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[CHAR_STRI_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[CHAR_STRI_ORDER]);
            }
        }
            return result;
    }
    return result;
}
IndexPairList _Index_SelectFloatNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPairList result;

    switch (filter.op) {
        case EQ: {
            IndexPair limit = _Index_SelectBoarderFloatNode(table, attr_name, filter);
            result.push_back(limit);
        } break;
        case NEQ: {
            BPT_FLOAT head;
            IndexPair limit = _Index_SelectBoarderFloatNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __FloatToString(head.key[i]));
                    if(line.second == limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case LT: {
            BPT_FLOAT head;
            IndexPair limit = _Index_SelectBoarderFloatNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __FloatToString(head.key[i]));
                    if(line.second < limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case GT: {
            BPT_FLOAT head;
            IndexPair limit = _Index_SelectBoarderFloatNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __FloatToString(head.key[i]));
                    if(line.second < limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case LTE: {
            BPT_FLOAT head;
            IndexPair limit = _Index_SelectBoarderFloatNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __FloatToString(head.key[i]));
                    if(line.second <= limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case GTE: {
            BPT_FLOAT head;
            IndexPair limit = _Index_SelectBoarderFloatNode(table, attr_name, filter);

            _IndexReadBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __FloatToString(head.key[i]));
                    if(line.second <= limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        }
            return result;
    }
    return result;
}

int DeleteIntIndex(Table table, std::string attr_name, IndexPair pair) {
    Index_Header index(table, attr_name);
    int i;
    BPT_INT a;
    int current = index.rootAddr;
    do
    {
        _IndexReadBlock(index, &a, current);
        int pairKey = __StringToInt(pair.second);
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0)
        {
            a.pointer[i] = -1;
            _IndexWriteBlock(index, &a, current);
            return 1;
        }
        current = a.pointer[i] ;
    }while(!a.isLeaf);
    return 0;
}
int DeleteCharIndex(Table table, std::string attr_name, IndexPair pair) {
    Index_Header index(table, attr_name);
    int i;
    BPT_CHAR a;
    int current = index.rootAddr;
    do
    {
        _IndexReadBlock(index, &a, current);
        std::string pairKey = pair.second;
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0)
        {
            a.pointer[i] = -1;
            _IndexWriteBlock(index, &a, current);
            return 1;
        }
        current = a.pointer[i] ;
    }while(!a.isLeaf);
    return 0;
}
int DeleteFloatIndex(Table table, std::string attr_name, IndexPair pair) {
    Index_Header index(table, attr_name);
    int i;
    BPT_FLOAT a;
    int current = index.rootAddr;
    do
    {
        _IndexReadBlock(index, &a, current);
        float pairKey = __StringToFloat(pair.second);
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0)
        {
            a.pointer[i] = -1;
            _IndexWriteBlock(index, &a, current);
            return 1;
        }
        current = a.pointer[i] ;
    }while(!a.isLeaf);
    return 0;
}
} // namespace db
