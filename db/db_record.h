#ifndef DB_DB_RECORD_H
#define DB_DB_RECORD_H

#include "db/db_type.h"
#include "db/db_global.h"

#include <string>

namespace db
{
    int InsertRecord(Table table,Tuple tuple);
    TupleList SelectRecordLinearOr(Table table,FilterList filters);
    TupleList SelectRecordLinear(Table table,FilterList filters);
    TupleList SelectRecordByList(Table table ,std::string attr_name,IndexPairList pairs);
    int DeleteRecordAll(Table table);
    TupleList DeleteRecordLinear(Table table,Filter filter);
    TupleList DeleteRecordByList(Table table,std::string attr_name,IndexPairList pairs);
    

    int __GetSizeOfOneTuple(Table table);
    int __GetNumOfTuplesInOneBlock(Table table);
    int __CheckOneTupleValid(Table table,char* content,int num);
    char __GetStateOfOneBlock(Table table,char* content);

    Tuple __ParserOneTuple(Table table,char *content,int num,int block);
    TupleList __ParserOneBlock(Table table,char *content,int block);
    int __DeleteOneTuple(Table table,char* content,int num);
    int __SetOneTuple(Table table,char *content,Tuple tuple,int num);

    int __FilterOneTupleByOneFilter(Table table,Tuple tuple,Filter filter);
    int __FilterOneTuple(Table table,Tuple tuple,FilterList filters,int option);
    int __StringToInt(std::string value);
    float __StringToFloat(std::string value);
    std::string __FloatToString(float val);
    std::string __IntToString(int val);
}

#endif  // DB_DB_RECORD_H
