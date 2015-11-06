#include "db/db_buffer.h"
#include "db/db_type.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <dirent.h>

using namespace std;
namespace db
{
    Buffer::Buffer()
    {
        __pointer=0;
        for(int i=0;i<100;i++)
        {
            __valid[i]=0;
            __buffer[i]=new char[4096];
        }
    }

    Buffer::~Buffer()
    {
        for(int i=0;i<100;i++)
        {
            delete[] __buffer[i];
        }
    }

    int Buffer::CreateTable(string table_name,string primary_key)
    {
        DIR *dirptr=NULL;
        if((dirptr=opendir("data"))==NULL)
        {
            mkdir("data",0755);
        }
        closedir(dirptr);

        string dir("data/"+table_name);
        mkdir(dir.c_str(),0755);
        fclose(fopen(__GetFilename(table_name,CATALOG).c_str(),"wb"));
        fclose(fopen(__GetFilename(table_name,DATA).c_str(),"wb"));
        fclose(fopen(__GetFilename(table_name,DATAPAGE).c_str(),"wb"));
        CreateIndex(table_name,primary_key);
        return 1;
    }

    int Buffer::CheckTable(string table_name)
    {
        string dir("data/"+table_name);
        DIR *dirptr=NULL;
        if((dirptr=opendir(dir.c_str()))!=NULL)
        {
            closedir(dirptr);
            return 1;
        }
        return 0;
    }

    int Buffer::DropTable(string table_name)
    {
        string dir("data/"+table_name);
        system(("rm -rf "+dir).c_str());

        return 1;
    }

    int Buffer::ReadCatalogBlock(string table_name,char* content)
    {
        int i=__IsInBuffer(table_name,CATALOG);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            string filename=__GetFilename(table_name,CATALOG);
            FILE* fp=fopen(filename.c_str(),"rb+");
            fseek(fp,0,SEEK_SET);
            fread(__buffer[i],4096,1,fp);
            __valid[i]=1;
            __table[i]=table_name;
            __type[i]=CATALOG;
            fclose(fp);
        }
        memcpy(content,__buffer[i],4096);
        return 1;
    }

    int Buffer::WriteCatalogBlock(string table_name,char* content)
    {
        int i=__IsInBuffer(table_name,CATALOG);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
        }
        memcpy(__buffer[i],content,4096);
        string filename=__GetFilename(table_name,CATALOG);
        FILE* fp=fopen(filename.c_str(),"rb+");
        fseek(fp,0,SEEK_SET);
        fwrite(__buffer[i],4096,1,fp);
        __valid[i]=1;
        __table[i]=table_name;
        __type[i]=CATALOG;
        fclose(fp);
        return 1;
    }

    int Buffer::CreateIndex(string table_name,string index_name)
    {
        //TODO
        return 0;
    }

    int Buffer::GetRootNumber(string table_name,string index_name)
    {
        //TODO
        return 0;
    }

    int Buffer::SetRootNumber(string table_name,string index_name,int block)
    {
        //TODO
        return 0;
    }

    int Buffer::GetHeightNumber(string table_name,string index_name)
    {
        //TODO
        return 0;
    }

    int Buffer::SetHeightNumber(string table_name,string index_name,int height)
    {
        //TODO
        return 0;
    }

    int Buffer::GetEmptyIndexBlock(string table_name,string index_name)
    {
        //TODO
        return 0;
    }

    int Buffer::ReadIndexBlock(string table_name,string index_name,int block,char* content)
    {
        //TODO
        return 0;
    }

    int Buffer::WriteIndexBlock(string table_name,string index_name,int block,char* content)
    {
        //TODO
        return 0;
    }

    int Buffer::ReadDataBlock(string table_name,int block,char* content)
    {
        //TODO
        return 0;
    }

    int Buffer::WriteDataBlock(string table_name,int block,char* content)
    {
        //TODO
        return 0;
    }

    int Buffer::GetAvailableDataBlock(string table_name)
    {
        //TODO
        return 0;
    }

    int Buffer::SetFullDataBlock(string table_name,int block)
    {
        //TODO
        return 0;
    }

    int Buffer::SetAvailableDataBlock(string table_name,int block)
    {
        //TODO
        return 0;
    }

    int Buffer::__IsInBuffer(string table_name,int type,string index_name,int block)
    {
        if(type==INDEX)
        {
            for(int i=0;i<100;i++)
            {
                if(__valid[i]==1&&__type[i]==type&&__table[i]==table_name&&__index[i]==index_name&&__block[i]==block)
                {
                    return i;
                }
            }
            return -1;
        }
        return -1;
    }

    int Buffer::__IsInBuffer(string table_name,int type,int block)
    {
        if(type==DATA||type==DATAPAGE)
        {
            for(int i=0;i<100;i++)
            {
                if(__valid[i]==1&&__type[i]==type&&__table[i]==table_name&&__block[i]==block)
                {
                    return i;
                }
            }
            return -1;
        }
        return -1;
    }

    int Buffer::__IsInBuffer(string table_name,int type)
    {
        if(type==CATALOG)
        {
            for(int i=0;i<100;i++)
            {
                if(__valid[i]==1&&__type[i]==type&&__table[i]==table_name)
                {
                    return i;
                }
            }
            return -1;
        }
        return -1;
    }

    int Buffer::__FindAvailableBufferBlock(string table_name)
    {
        for(int i=0;i<100;i++)
        {
            if(__valid[i]==0||__table[i]!=table_name)
                return i;
        }
        int old=__pointer;
        __pointer=(__pointer+1)%100;
        return old;
    }

    string Buffer::__GetFilename(string table_name,int type,string index_name)
    {
        string path("data/"+table_name+"/");
        switch(type)
        {
            case INDEX:
                return path+index_name+".idx";
                break;
            default:
                return "";
        }
    }

    string Buffer::__GetFilename(string table_name,int type)
    {
        string path("data/"+table_name+"/");
        switch(type)
        {
            case CATALOG:
                return path+"catalog";
                break;
            case DATA:
                return path+"data";
                break;
            case DATAPAGE:
                return path+"data.dpg";
                break;
            default:
                return "";
        }
    }
}
