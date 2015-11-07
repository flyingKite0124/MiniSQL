#include "db/db_record.h"
#include "db/db_type.h"
#include "db/db_global.h"

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
        char *content=new char[4096];
        int block=BufferDelegate.GetAvailableDataBlock(table.GetName());
        BufferDelegate.ReadDataBlock(table.GetName(),block,content);
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        int i;
        for(i=0;i<tuple_num;i++)
        {
            if(__CheckOneTupleValid(table,content,i)==0)
                break;
        }
        __SetOneTuple(table,content,tuple,i);
        BufferDelegate.WriteDataBlock(table.GetName(),block,content);
        char state=__GetStateOfOneBlock(table,content);
        BufferDelegate.SetDataBlockState(table.GetName(),block,state);
        delete[] content;

        return block;
    }

    TupleList SelectRecordLinearOr(Table table,FilterList filters)
    {
        return __LinearSelect(table,filters,OR);
    }

    TupleList SelectRecordLinear(Table table,FilterList filters)
    {
        return __LinearSelect(table,filters,AND);
    }

    TupleList SelectRecordByList(Table table ,string attr_name,IndexPairList pairs,FilterList filters)
    {
        TupleList tuples;
        Tuple tuple;
        char content[4096];
        int pairs_num=pairs.size();
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        for(int i=0;i<pairs_num;i++)
        {
            Filter filter(attr_name+" = "+pairs[i].second);
            int block=pairs[i].first;
            BufferDelegate.ReadDataBlock(table.GetName(),block,content);
            for(int j=0;j<tuple_num;j++)
            {
                if(__CheckOneTupleValid(table,content,j))
                {
                    tuple=__ParserOneTuple(table,content,j,block);
                    if(__FilterOneTupleByOneFilter(table,tuple,filter)&&__FilterOneTuple(table,tuple,filters,AND))
                        tuples.push_back(tuple);
                }
            }
        }
        return tuples;
    }

    int DeleteRecordAll(Table table)
    {
        BufferDelegate.RecreateDataFile(table.GetName());
        return 1;
    }

    TupleList DeleteRecordLinear(Table table,Filter filter)
    {
        TupleList tuples;
        Tuple tuple;
        char data_page_content[4096];
        char data_content[4096];

        int data_page_blocks=BufferDelegate.GetDataPageFileSize(table.GetName());
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        for(int i=0;i<data_page_blocks;i++)
        {
            BufferDelegate.ReadDataPageBlock(table.GetName(),i,data_page_content);
            vector<int> used_list;
            char state;
            for(int j=0;j<4096;j++)
            {
                memcpy(&state,data_page_content+j,sizeof(char));
                if(state!=0)
                    used_list.push_back(j);
            }
            for(int j=0;j<used_list.size();j++)
            {
                BufferDelegate.ReadDataBlock(table.GetName(),i*4096+j,data_content);
                for(int k=0;k<tuple_num;k++)
                {
                    if(__CheckOneTupleValid(table,data_content,k))
                    {
                        tuple=__ParserOneTuple(table,data_content,k,i*4096+j);
                        if(__FilterOneTupleByOneFilter(table,tuple,filter))
                        {
                            tuples.push_back(tuple);
                            __DeleteOneTuple(table,data_content,k);
                            state=__GetStateOfOneBlock(table,data_content);
                            BufferDelegate.SetDataBlockState(table.GetName(),i*4096+j,state);
                        }
                    }
                }
                BufferDelegate.WriteDataBlock(table.GetName(),i*4096+j,data_content);
            }
        }
        return tuples;
    }

    TupleList DeleteRecordByList(Table table,string attr_name,IndexPairList pairs)
    {
        TupleList tuples;
        Tuple tuple;
        char content[4096];
        int pairs_num=pairs.size();
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        for(int i=0;i<pairs_num;i++)
        {
            Filter filter(attr_name+" = "+pairs[i].second);
            int block=pairs[i].first;
            BufferDelegate.ReadDataBlock(table.GetName(),block,content);
            for(int j=0;j<tuple_num;j++)
            {
                if(__CheckOneTupleValid(table,content,j))
                {
                    tuple=__ParserOneTuple(table,content,j,block);
                    if(__FilterOneTupleByOneFilter(table,tuple,filter))
                    {
                        tuples.push_back(tuple);
                        __DeleteOneTuple(table,content,j);
                        char state=__GetStateOfOneBlock(table,content);
                        BufferDelegate.SetDataBlockState(table.GetName(),i*4096+j,state);
                        break;
                    }
                }
            }
            BufferDelegate.WriteDataBlock(table.GetName(),block,content);
        }
        return tuples;
    }

    TupleList __LinearSelect(Table table,FilterList filters,int option)
    {
        TupleList tuples;
        Tuple tuple;
        char data_page_content[4096];
        char data_content[4096];

        int data_page_blocks=BufferDelegate.GetDataPageFileSize(table.GetName());
        int tuple_num=__GetNumOfTuplesInOneBlock(table);
        for(int i=0;i<data_page_blocks;i++)
        {
            BufferDelegate.ReadDataPageBlock(table.GetName(),i,data_page_content);
            vector<int> used_list;
            char state;
            for(int j=0;j<4096;j++)
            {
                memcpy(&state,data_page_content+j,sizeof(char));
                if(state!=0)
                    used_list.push_back(j);
            }
            for(int j=0;j<used_list.size();j++)
            {
                BufferDelegate.ReadDataBlock(table.GetName(),i*4096+j,data_content);
                for(int k=0;k<tuple_num;k++)
                {
                    if(__CheckOneTupleValid(table,data_content,k))
                    {
                        tuple=__ParserOneTuple(table,data_content,k,i*4096+j);
                        if(__FilterOneTuple(table,tuple,filters,option))
                            tuples.push_back(tuple);
                    }
                }
            }
        }
        return tuples;
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
    
    char __CheckOneTupleValid(Table table,char* content,int num)
    {
        int tuple_size=__GetSizeOfOneTuple(table);
        char valid;
        memcpy(&valid,content+num*(tuple_size+1),sizeof(char));
        return valid;
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
            state=EMPTY;
        else if(count<tuple_num&&count>0)
            state=USED;
        else if(count==tuple_num)
            state=FULL;
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
                memcpy(content+num*(tuple_size+1)+offset,&val,attr.size);
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


