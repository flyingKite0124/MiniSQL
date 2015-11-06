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
}

#endif  // DB_DB_RECORD_H
