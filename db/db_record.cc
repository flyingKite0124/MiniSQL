#include "db/db_record.h"
#include <string>

using namespace std;
namespace db
{
    int InsertRecord(Table table,Tuple tuple)
    {
        //TODO
        return 0;
    }
    TupleList SelectRecordLinearOr(Table table,FilterList filters)
    {
        //TODO
    }

    TupleList SelectRecordLinear(Table table,FilterList filters)
    {
        //TODO
    }

    TupleList SelectRecordByList(Table table ,string attr_name,IndexPairList pairs)
    {
        //TODO
    }

    int DeleteRecordAll(Table table)
    {
        //TODO
        return 0;
    }

    TupleList DeleteRecordLinear(Table table,Filter filter)
    {
        //TODO
    }

    TupleList DeleteRecordByList(Table table,string attr_name,IndexPairList pairs)
    {
        //TODO
    }
}
