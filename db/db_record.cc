#include "db/db_record.h"
#include "db/db_type.h"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

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

    int __GetSizeOfOneTuple(Table table)
    {
        vector<Attribute> attrs= table.GetAttributes();
        int size=0;
        for(auto& attr:attrs)
        {
            if(attr.type==TYPE_CHAR)
                size+=attr.size+1;
            else
                size+=attr.size;
        }
        return size;
    }

    int __GetNumOfTuplesInOneBlock(Table table)
    {
        return 4096/(__GetSizeOfOneTuple(table)+1);
    }
    
    int __CheckOneTupleValid(Table table,char* content,int num)
    {
        int tuple_size=__GetSizeOfOneTuple(table);
        char valid;
        memcpy(&valid,content+num*(tuple_size+1),sizeof(char));
        return (int)valid;
    }

    char __GetStateOfOneBlock(Table table,char *content)
    {
        char state;
        int count=0;
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        for(int i=0;i<tuple_num;i++)
        {
            if(__CheckOneTupleValid(table,content,i))
                    count++;
        }
        if(count==0)
            state=0;
        else if(count<tuple_num)
            state=1;
        else
            state=2;
        return state;
    }

    Tuple __ParserOneTuple(Table table,char* content,int num,int block)
    {
        vector<Attribute> attrs= table.GetAttributes();
        Tuple tuple;
        int offset=1;
        int tuple_size=__GetSizeOfOneTuple(table);

        for(auto& attr:attrs)
        {
            string value;
            if(attr.type==TYPE_INT)
            {
                int val;
                memcpy(&val,content+num*(tuple_size+1)+offset,attr.size);
                offset+=attr.size;
                value=__IntToString(val);
            }
            else if(attr.type==TYPE_FLOAT)
            {
                float val;
                memcpy(&val,content+num*(tuple_size+1)+offset,attr.size);
                offset+=attr.size;
                value=__FloatToString(val);
            }
            else if(attr.type==TYPE_CHAR)
            {
                char *val=new char[attr.size+1];
                strcpy(val,content+num*(tuple_size+1)+offset);
                offset+=attr.size+1;
                value=val;
                delete[] val;
            }
            tuple.first=block;
            tuple.second.push_back(value);
        }
        return tuple;
    }
   
    TupleList __ParserOneBlock(Table table,char *content,int block)
    {
        TupleList tuples;
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        for(int i=0;i<tuple_num;i++)
        {
            if(__CheckOneTupleValid(table,content,i))
            {
                Tuple tuple=__ParserOneTuple(table,content,i,block);
                tuples.push_back(tuple);
            }
        }
        return tuples;
    }

    int __DeleteOneTuple(Table table,char* content,int num)
    {
        int tuple_size=__GetSizeOfOneTuple(table);
        char valid=0;
        memcpy(content+num*(tuple_size+1),&valid,sizeof(char));
        return 1;
    }

    int __SetOneTuple(Table table,char *content,Tuple tuple,int num)
    {
        vector<Attribute> attrs= table.GetAttributes();
        int offset=0;
        int tuple_size=__GetSizeOfOneTuple(table);
        
        
        char valid=1;
        memcpy(content+num*(tuple_size+1)+offset,&valid,sizeof(char));
        offset+=1;
        for(int i=0;i<attrs.size();i++)
        {
            Attribute attr=attrs[i];
            if(attr.type==TYPE_INT)
            {
                int val=__StringToInt(tuple.second[i]);
                memcpy(content+num*(tuple_size+1)+offset,&val,attr.size);
                offset+=attr.size;
            }
            else if(attr.type==TYPE_FLOAT)
            {
                float val=__StringToFloat(tuple.second[i]);
                memcpy(&val,content+num*(tuple_size+1)+offset,attr.size);
                offset+=attr.size;
            }
            else if(attr.type==TYPE_CHAR)
            {
                strcpy(content+num*(tuple_size+1)+offset,tuple.second[i].c_str());
                offset+=attr.size+1;
            }
        }
        return 1;
    }

    int __FilterOneTupleByOneFilter(Table table,Tuple tuple,Filter filter)
    {
        int flag=0;
        vector<Attribute> attrs= table.GetAttributes();
        int index;
        for(index=0;index<attrs.size();index++)
        {
            if(attrs[index].name==filter.key)
                break;
        }
        if(attrs[index].type==TYPE_INT)
        {
            int left=__StringToInt(tuple.second[index]);
            int right=__StringToInt(filter.value);
            switch(filter.op)
            {
                case LT:if(left<right)
                            flag=1;
                        break;
                case LTE:if(left<=right)
                             flag=1;
                         break;
                case GT:if(left>right)
                            flag=1;
                        break;
                case GTE:if(left>=right)
                            flag=1;
                        break;
                case NEQ:if(left!=right)
                            flag=1;
                        break;
                case EQ:if(left==right)
                            flag=1;
                        break;
            }
        }
        else if(attrs[index].type==TYPE_FLOAT)
        {
            float left=__StringToFloat(tuple.second[index]);
            float right=__StringToFloat(filter.value);
            switch(filter.op)
            {
                case LT:if(left<right)
                            flag=1;
                        break;
                case LTE:if(left<=right)
                             flag=1;
                         break;
                case GT:if(left>right)
                            flag=1;
                        break;
                case GTE:if(left>=right)
                            flag=1;
                        break;
                case NEQ:if(left!=right)
                            flag=1;
                        break;
                case EQ:if(left==right)
                            flag=1;
                        break;
            }
        }
        else if(attrs[index].type==TYPE_INT)
        {
            string left=tuple.second[index];
            string right=filter.value;
            switch(filter.op)
            {
                case LT:if(strcmp(left.c_str(),right.c_str())<0)
                            flag=1;
                        break;
                case LTE:if(strcmp(left.c_str(),right.c_str())<=0)
                             flag=1;
                         break;
                case GT:if(strcmp(left.c_str(),right.c_str())>0)
                            flag=1;
                        break;
                case GTE:if(strcmp(left.c_str(),right.c_str())>=0)
                            flag=1;
                        break;
                case NEQ:if(strcmp(left.c_str(),right.c_str())!=0)
                            flag=1;
                        break;
                case EQ:if(strcmp(left.c_str(),right.c_str())==0)
                            flag=1;
                        break;
            }
        }
        return flag;
    }

    int __FilterOneTuple(Table table,Tuple tuple,FilterList filters,int option)
    {
        int flag,one_flag;
        if(option==OR)
            flag=0;
        else
            flag=1;
        for(int i=0;i<filters.size();i++)
        {
            one_flag=__FilterOneTupleByOneFilter(table,tuple,filters[i]);
            if(option==OR)
                flag|=one_flag;
            else
                flag&=one_flag;
        }
        return flag;
    }

    int __StringToInt(string value)
    {
        int val;
        stringstream ss;
        ss << value;
        ss >> val;
        return val;
    }

    float __StringToFloat(string value)
    {
        float val;
        stringstream ss;
        ss << value;
        ss >> val;
        return val;
    }

    string __FloatToString(float val)
    {
        string value;
        stringstream ss;
        ss << val;
        ss >> value;
        return value;
    }

    string __IntToString(int val)
    {
        string value;
        stringstream ss;
        ss << val;
        ss >> value;
        return value;
    }
}


