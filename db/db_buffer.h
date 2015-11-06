#ifndef DB_DB_BUFFER_H
#define DB_DB_BUFFER_H 

#include <string>
namespace db
{
    class Buffer
    {
        public:   
            Buffer();
            ~Buffer();
            int CreateTable(std::string table_name,std::string primary_key);
            int DropTable(std::string table_name);
            int CheckTable(std::string table_name);

            int ReadCatalogBlock(std::string table_name,char* content);
            int WriteCatalogBlock(std::string table_name,char* content);

            int CreateIndex(std::string table_name,std::string index);
            int DropIndex(std::string table_name,std::string index);

            int GetRootNumber(std::string table_name,std::string index_name);
            int SetRootNumber(std::string table_name,std::string index_name,int block);
            int GetHeightNumber(std::string table_name,std::string index_name);
            int SetHeightNumber(std::string table_name,std::string index_name,int height);
            int GetEmptyIndexBlock(std::string table_name,std::string index_name);
            int ReadIndexBlock(std::string table_name,std::string index_name,int block,char* content);
            int WriteIndexBlock(std::string table_name,std::string index_name,int block,char* content);

            int ReadDataBlock(std::string table_name,int block,char* content);
            int WriteDataBlock(std::string table_name,int block,char* content);
            int GetAvailableDataBlock(std::string table_name);
            int SetFullDataBlock(std::string table_name,int block);
            int SetAvailableDataBlock(std::string table_name,int block);
        private:
            unsigned char space[100][4000];
            std::string table_name[100];
            std::string index_name[100];
            int type[100];
            int block[100];
            int valid[100];
            int pointer;
            int __IsInBuffer(std::string table_name,int type,std::string index_name,int block);
            int __IsInBuffer(std::string table_name,int type,int block);
            int __IsInBuffer(std::string table_name,int type);
            int __FindAvailableBufferBlock(std::string table_name);
            std::string __GetFilename(std::string table_name,int type,std::string index_name);
            std::string __GetFilename(std::string table_name,int type);
    };
}
#endif //DB_DB_BUFFER_H
