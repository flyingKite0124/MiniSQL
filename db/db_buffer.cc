#include "db/db_buffer.h"
#include "db/db_type.h"

#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <string>
#include <dirent.h>

using namespace std;
namespace db
{
    Buffer::Buffer()
    {
        for(int i=0;i<100;i++)
        {
            valid[i]=0;
        }
        pointer=0;
    }

    Buffer::~Buffer()
    {
    }

    int Buffer::CreateTable(string table_name,string primary_key)
    {
        string dir("data/"+table_name);
        mkdir(dir.c_str(),0755);
        fopen(__GetFilename(table_name,CATALOG).c_str(),"wb");
        fopen(__GetFilename(table_name,DATA).c_str(),"wb");
        fopen(__GetFilename(table_name,DATAPAGE).c_str(),"wb");
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
        //TODO
        return 0;
    }

    int Buffer::WriteCatalogBlock(string table_name,char* content)
    {
        //TODO
        return 0;
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
        //TODO
        return 0;
    }

    int Buffer::__IsInBuffer(string table_name,int type,int block)
    {
        //TODO
        return 0;
    }

    int Buffer::__IsInBuffer(string table_name,int type)
    {
        //TODO
        return 0;
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
