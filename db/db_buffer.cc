#include "db/db_buffer.h"

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
    }

    Buffer::~Buffer()
    {
    }

    int Buffer::CreateTable(string table_name,string primary_key)
    {
        string dir("data/"+table_name);
        mkdir(dir.c_str(),0755);
        fopen(__GetFilename(table_name,CATALOG).c_str(),"w");
        fopen(__GetFilename(table_name,DATA).c_str(),"w");
        fopen(__GetFilename(table_name,DATAPAGE).c_str(),"w");
        fopen(__GetFilename(table_name,INDEX,primary_key).c_str(),"w");
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
        
        return 0;
    }


    string Buffer::__GetFilename(string table_name,int type,string index)
    {
        string path("data/"+table_name+"/");
        string filename();
        switch(type)
        {
            case INDEX:
                return path+index+".idx";
                break;
        }
    }

    string Buffer::__GetFilename(string table_name,int type)
    {
        string path("data/"+table_name+"/");
        string filename();
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
        }
    }

}
