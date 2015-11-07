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

int CreateIndex(Table table, std::string attr_name) {
    Index_Header index(table, attr_name);

    // Reset Root Nodes
    BPT_INT R;
    R.isLeaf = true;
    memset(R.key, 0, sizeof(R.key));
    memset(R.pointer, 0, sizeof(R.pointer));
    R.countKey = 0;
    _IndexWriteIntBlock(index, &R, index.rootAddr);

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

void _IndexWriteIntBlock(Index_Header idx, BPT_INT *node, int pos) {
    char *block = new char[4096];
    memcpy(block, (char*)node, sizeof(BPT_INT));
    BufferDelegate.WriteIndexBlock(idx.tableName, idx.attrName, pos, block);
}
void _IndexReadIntBlock(Index_Header idx, BPT_INT *node, int pos) {
    char *block = new char[4096];
    BufferDelegate.ReadIndexBlock(idx.tableName, idx.attrName, pos, block);
    memcpy((char*)node, block, sizeof(BPT_INT));
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

    _IndexWriteIntBlock(idx, &t, address);

}

void _Insert_IndexforInt(Index_Header idx, int current, IndexPair pair) {
    BPT_INT node;
    _IndexReadIntBlock(idx, &node, current);

    int	i, pairKey = __StringToInt(pair.second);
    for(i=0 ; i<node.countKey && node.key[i]<pairKey; i++);

    if(i<node.countKey && node.isLeaf && node.key[i]==pairKey) {
        if(node.pointer[i] == -1) node.pointer[i] = pair.first;
        else return ;
    }

    if(!node.isLeaf)	//如果不是叶节点
    {
        BPT_INT back;
        _IndexReadIntBlock(idx, &back, node.pointer[i]);

        if(back.countKey == INT_FLOAT_ORDER) {
            //如果x的子节点已满，则这个子节点分裂
            _Index_SplitNode(idx, node, back, i);
            _IndexWriteIntBlock(idx, &node, current);
            _IndexWriteIntBlock(idx, &back, node.pointer[i]);
        }
        if(pairKey<=node.key[i] || i==node.countKey) {
            _Insert_IndexforInt(idx, node.pointer[i], pair);
        } else {
            _Insert_IndexforInt(idx, node.pointer[i+1], pair);
        }

    }
    else {			//如果是叶节点,则直接将关键字插入key数组中
        for(int j = node.countKey ; j > i ; j--) {
            node.key[j] = node.key[j-1] ;
            node.pointer[j] = node.pointer[j-1] ;
        }
        node.key[i] = pairKey ;
        node.countKey++;
        node.pointer[i] = pair.first;

        _IndexWriteIntBlock(idx, &node, current);
    }
}
int InsertIndex(Table table, std::string attr_name,IndexPair pair) {
    Index_Header index(table, attr_name);

    BPT_INT node;
    _IndexReadIntBlock(index, &node, index.rootAddr);

    if(node.countKey == INT_FLOAT_ORDER) {
        BPT_INT nR;
        nR.isLeaf = false;
        nR.countKey = 0;
        nR.pointer[0] = index.rootAddr;

        _Index_SplitNode(index, nR, node, 0);
        _IndexWriteIntBlock(index, &node, index.rootAddr);

        index.rootAddr = BufferDelegate.GetEmptyIndexBlock(index.tableName, index.attrName);
        _IndexWriteIntBlock(index, &nR, index.rootAddr);
        BufferDelegate.SetRootNumber(index.tableName, index.attrName, index.rootAddr);
    }
    _Insert_IndexforInt(index, index.rootAddr, pair);

    return 1;
}

IndexPair _Index_SelectBoarderIntNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPair result;

    int i;  BPT_INT a;
    int current = index.rootAddr;
    do
    {
        _IndexReadIntBlock(index, &a, current);
        int pairKey = __StringToInt(filter.value);
        for(i = 0; i < a.countKey && pairKey > a.key[i]; i++);

        if(i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i] > 0) {
            result = std::pair<int, std::string>(a.pointer[i], __IntToString(a.key[i]);
//            cout << a.key[i] << "-" << a.pointer[i] << endl;
            return result;
        }
        current = a.pointer[i] ;

    }while(!a.isLeaf);
    return result;
}

IndexPairList _Index_SearchIntNode(Table table, std::string attr_name, Filter filter) {
    Index_Header index(table, attr_name);
    IndexPairList result;

    switch (filter.op) {
        case EQ: {
            IndexPair limit = _Index_SearchBoarderIntNode(table, attr_name, filter);
            result.push_back(limit);
        } break;
        case NEQ: {
            BPT_INT head;
            IndexPair limit = _Index_SearchBoarderIntNode(table, attr_name, filter);

            _IndexReadIntBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadIntBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]);
                    if(line.second == limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadIntBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case LT: {
            BPT_INT head;
            IndexPair limit = _Index_SearchBoarderIntNode(table, attr_name, filter);

            _IndexReadIntBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadIntBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]);
                    if(line.second < limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadIntBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
        } break;
        case GT: {
            BPT_INT head;
            IndexPair limit = _Index_SearchBoarderIntNode(table, attr_name, filter);

            _IndexReadIntBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadIntBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]);
                    if(line.second < limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadIntBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case LTE: {
            BPT_INT head;
            IndexPair limit = _Index_SearchBoarderIntNode(table, attr_name, filter);

            _IndexReadIntBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadIntBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]);
                    if(line.second <= limit.second) result.push_back(line);
                    else break;
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadIntBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        } break;
        case GTE: {
            BPT_INT head;
            IndexPair limit = _Index_SearchBoarderIntNode(table, attr_name, filter);

            _IndexReadIntBlock(index, &head, index.rootAddr);
            while(!head.isLeaf) {
                _IndexReadIntBlock(index, &head, head.pointer[0]);
            }

            while(1) {
                for(int i = 0; i < head.countKey; i++) {
                    IndexPair line = std::pair<int, std::string>(head.pointer[i], __IntToString(head.key[i]);
                    if(line.second <= limit.second) continue;
                    else result.push_back(line);
                }

                if(head.pointer[INT_FLOAT_ORDER] == 0 )
                    break;

                _IndexReadIntBlock(index, &head, head.pointer[INT_FLOAT_ORDER]);
            }
        }
        return result;
    }

}

int DeleteIndex(Table table, std::string attr_name, IndexPair pair) {
    Index_Header index(table, attr_name);

    int i;
    BPT_INT a;
    int current = index.rootAddr;

    do
    {
        _IndexReadIntBlock(index, &a, current);
        int pairKey = __StringToInt(pair.second);
        for(i = 0 ; i < a.countKey && pairKey > a.key[i] ; i++ );

        if( i < a.countKey && a.isLeaf && pairKey == a.key[i] && a.pointer[i]>0)		//在B+树叶节点找到了等值的关键字
        {
           a.pointer[i] = -1;				//返回该关键字所对应的记录的地址
            _IndexWriteIntBlock(index, &a, current);
        }
        current = a.pointer[i] ;

    }while(!a.isLeaf);
    return 0;
}
} // namespace db