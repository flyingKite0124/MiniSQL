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
        for(int i=0;i<100000;i++)
        {
            __valid[i]=0;
            __buffer[i]=new char[4096];
        }
        zero_block=new char[4096];
        memset(zero_block,0,4096);
    }

    Buffer::~Buffer()
    {
        for(int i=0;i<100000;i++)
        {
            delete[] __buffer[i];
        }
        delete[] zero_block;
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
        char *first_block=new char[4096];
        memset(first_block,0,4096);
        memset(first_block,2,1);
        WriteDataPageBlock(table_name,0,first_block);
        delete[] first_block;
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
            __valid[i]=1;
            __table[i]=table_name;
            __type[i]=CATALOG;
        }
        memcpy(__buffer[i],content,4096);
        string filename=__GetFilename(table_name,CATALOG);
        FILE* fp=fopen(filename.c_str(),"rb+");
        fseek(fp,0,SEEK_SET);
        fwrite(__buffer[i],4096,1,fp);
        fclose(fp);
        return 1;
    }

    int Buffer::CreateIndex(string table_name,string index_name)
    {
        string filename=__GetFilename(table_name,INDEX,index_name);
        fclose(fopen(filename.c_str(),"wb"));
        char *first_block=new char[4096];
        memset(first_block,0,4096);
        int num=1;
        memcpy(first_block,&num,sizeof(int));
        num=0;
        memcpy(first_block+sizeof(int),&num,sizeof(int));
        num=2;
        memcpy(first_block+2*sizeof(int),&num,sizeof(int));

        WriteIndexBlock(table_name,index_name,0,first_block);
        char *root_block=new char[4096];
        memcpy(root_block,zero_block,4096);
        char leaf=1;
        memcpy(root_block,&leaf,sizeof(char));
        WriteIndexBlock(table_name,index_name,1,root_block);

        delete[] root_block;
        delete[] first_block;

        return 1;
    }

    int Buffer::DropIndex(string table_name,string index_name)
    {
        string filename=__GetFilename(table_name,INDEX,index_name);
        system(("rm "+filename).c_str());
        return 1;
    }

    int Buffer::GetRootNumber(string table_name,string index_name)
    {
        char *first_block=new char[4096];
        ReadIndexBlock(table_name,index_name,0,first_block);
        int root;
        memcpy(&root,first_block,sizeof(int));
        delete[] first_block;
        return root;
    }

    int Buffer::SetRootNumber(string table_name,string index_name,int block)
    {
        char *first_block=new char[4096];
        ReadIndexBlock(table_name,index_name,0,first_block);
        memcpy(first_block,&block,sizeof(int));
        WriteIndexBlock(table_name,index_name,0,first_block);
        delete[] first_block;
        return 1;
    }

    int Buffer::GetHeightNumber(string table_name,string index_name)
    {
        char *first_block=new char[4096];
        ReadIndexBlock(table_name,index_name,0,first_block);
        int height;
        memcpy(&height,first_block+sizeof(int),sizeof(int));
        delete[] first_block;
        return height;
    }

    int Buffer::SetHeightNumber(string table_name,string index_name,int height)
    {
        char *first_block=new char[4096];
        ReadIndexBlock(table_name,index_name,0,first_block);
        memcpy(first_block+sizeof(int),&height,sizeof(int));
        WriteIndexBlock(table_name,index_name,0,first_block);
        delete[] first_block;
        return 1;
    }

    int Buffer::GetEmptyIndexBlock(string table_name,string index_name)
    {
        char *first_block=new char[4096];
        ReadIndexBlock(table_name,index_name,0,first_block);
        int empty;
        memcpy(&empty,first_block+2*sizeof(int),sizeof(int));
        int next=empty+1;
        memcpy(first_block+2*sizeof(int),&next,sizeof(int));
        WriteIndexBlock(table_name,index_name,0,first_block);
        delete[] first_block;
        return empty;
    }

    int Buffer::ReadIndexBlock(string table_name,string index_name,int block,char* content)
    {
        int i=__IsInBuffer(table_name,INDEX,index_name,block);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            string filename=__GetFilename(table_name,INDEX,index_name);
            FILE* fp=fopen(filename.c_str(),"rb+");
            fseek(fp,block*4096,SEEK_SET);
            fread(__buffer[i],4096,1,fp);
            __valid[i]=1;
            __table[i]=table_name;
            __index[i]=index_name;
            __type[i]=INDEX;
            __block[i]=block;
            fclose(fp);
        }
        memcpy(content,__buffer[i],4096);
        return 1;
    }

    int Buffer::WriteIndexBlock(string table_name,string index_name,int block,char* content)
    {
        int i=__IsInBuffer(table_name,INDEX,index_name,block);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            __valid[i]=1;
            __table[i]=table_name;
            __index[i]=index_name;
            __type[i]=INDEX;
            __block[i]=block;
        }
        memcpy(__buffer[i],content,4096);
        string filename=__GetFilename(table_name,INDEX,index_name);
        FILE* fp=fopen(filename.c_str(),"rb+");
        fseek(fp,block*4096,SEEK_SET);
        fwrite(__buffer[i],4096,1,fp);
        fclose(fp);
        return 1;
    }

    int Buffer::RecreateDataFile(string table_name)
    {
        string filename=__GetFilename(table_name,DATA);
        fclose(fopen(filename.c_str(),"wb"));
        filename=__GetFilename(table_name,DATAPAGE);
        fclose(fopen(filename.c_str(),"wb"));
        char *first_block=new char[4096];
        memset(first_block,0,4096);
        memset(first_block,2,1);
        WriteDataPageBlock(table_name,0,first_block);
        delete[] first_block;
        return 1;
    }

    int Buffer::ReadDataBlock(string table_name,int block,char* content)
    {
        int i=__IsInBuffer(table_name,DATA,block);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            string filename=__GetFilename(table_name,DATA);
            FILE* fp=fopen(filename.c_str(),"rb+");
            fseek(fp,block*4096,SEEK_SET);
            fread(__buffer[i],4096,1,fp);
            __valid[i]=1;
            __table[i]=table_name;
            __type[i]=DATA;
            __block[i]=block;
            fclose(fp);
        }
        memcpy(content,__buffer[i],4096);
        return 1;
    }

    int Buffer::WriteDataBlock(string table_name,int block,char* content)
    {
        int i=__IsInBuffer(table_name,DATA,block);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            __valid[i]=1;
            __table[i]=table_name;
            __type[i]=DATA;
            __block[i]=block;
        }
        memcpy(__buffer[i],content,4096);
        string filename=__GetFilename(table_name,DATA);
        FILE* fp=fopen(filename.c_str(),"rb+");
        fseek(fp,block*4096,SEEK_SET);
        fwrite(__buffer[i],4096,1,fp);
        fclose(fp);
        return 1;
    }

    int Buffer::ReadDataPageBlock(string table_name,int block,char* content)
    {
        int i=__IsInBuffer(table_name,DATAPAGE,block);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            string filename=__GetFilename(table_name,DATAPAGE);
            FILE* fp=fopen(filename.c_str(),"rb+");
            fseek(fp,block*4096,SEEK_SET);
            fread(__buffer[i],4096,1,fp);
            __valid[i]=1;
            __table[i]=table_name;
            __type[i]=DATAPAGE;
            __block[i]=block;
            fclose(fp);
        }
        memcpy(content,__buffer[i],4096);
        return 1;
    }

    int Buffer::WriteDataPageBlock(string table_name,int block,char* content)
    {
        int i=__IsInBuffer(table_name,DATAPAGE,block);
        if(i==-1)
        {
            i=__FindAvailableBufferBlock(table_name);
            __valid[i]=1;
            __table[i]=table_name;
            __type[i]=DATAPAGE;
            __block[i]=block;
        }
        memcpy(__buffer[i],content,4096);
        string filename=__GetFilename(table_name,DATAPAGE);
        FILE* fp=fopen(filename.c_str(),"rb+");
        fseek(fp,block*4096,SEEK_SET);
        fwrite(__buffer[i],4096,1,fp);
        fclose(fp);
        return 1;
    }

    int Buffer::GetAvailableDataBlock(string table_name)
    {
        char available;
        char *content=new char[4096];
        int size=GetDataPageFileSize(table_name);
        for(int i=0;i<size;i++)
        {
            ReadDataPageBlock(table_name,i,content);
            for(int j=0;j<4096;j++)
            {
                memcpy(&available,content+j*sizeof(char),sizeof(char));
                if(available==EMPTY||available==USED)
                {
                    delete[] content;
                    int block=i*4096+j;
                    if(block>=GetDataFileSize(table_name))
                        WriteDataBlock(table_name,block,zero_block);
                    return block;
                }
            }
        }
        WriteDataPageBlock(table_name,size,zero_block);
        delete[] content;
        int block=size*4096;
        WriteDataBlock(table_name,block,zero_block);
        return block;
    }

    int Buffer::SetDataBlockState(string table_name,int block,char state)
    {
        char *content=new char[4096];
        int page_block=block/4096;
        int offset=block%4096;
        ReadDataPageBlock(table_name,page_block,content);
        memcpy(content+offset*sizeof(char),&state,sizeof(char));
        WriteDataPageBlock(table_name,page_block,content);
        delete[] content;
        return 1;
    }

    int Buffer::GetDataFileSize(string table_name)
    {
        string filename=__GetFilename(table_name,DATA);
        struct stat statbuff;
        stat(filename.c_str(),&statbuff);
        return statbuff.st_size/4096;
    }

    int Buffer::GetDataPageFileSize(string table_name)
    {
        string filename=__GetFilename(table_name,DATAPAGE);
        struct stat statbuff;
        stat(filename.c_str(),&statbuff);
        return statbuff.st_size/4096;
    }

    int Buffer::__IsInBuffer(string table_name,int type,string index_name,int block)
    {
        if(type==INDEX)
        {
            for(int i=0;i<100000;i++)
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
            for(int i=0;i<100000;i++)
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
            for(int i=0;i<100000;i++)
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
        for(int i=0;i<100000;i++)
        {
            if(__valid[i]==0||__table[i]!=table_name)
                return i;
        }
        int old=__pointer;
        __pointer=(__pointer+1)%1000;
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
