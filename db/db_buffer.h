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

            int CreateIndex(std::string table_name,std::string index_name);
            int DropIndex(std::string table_name,std::string index_name);

            int GetRootNumber(std::string table_name,std::string index_name);
            int SetRootNumber(std::string table_name,std::string index_name,int block);
            int GetHeightNumber(std::string table_name,std::string index_name);
            int SetHeightNumber(std::string table_name,std::string index_name,int height);
            int GetEmptyIndexBlock(std::string table_name,std::string index_name);
            int ReadIndexBlock(std::string table_name,std::string index_name,int block,char* content);
            int WriteIndexBlock(std::string table_name,std::string index_name,int block,char* content);

            int RecreateDataFile(std::string table_name);
            int ReadDataBlock(std::string table_name,int block,char* content);
            int WriteDataBlock(std::string table_name,int block,char* content);

            int ReadDataPageBlock(std::string table_name,int block,char* content);
            int WriteDataPageBlock(std::string table_name,int block,char* content);

            int GetAvailableDataBlock(std::string table_name);
            int SetDataBlockState(std::string table_name,int block,char state);

            int GetDataFileSize(std::string table_name);
            int GetDataPageFileSize(std::string table_name);

            char *zero_block;
        private:
            char *__buffer[100000];
            std::string __table[100000];
            std::string __index[100000];
            int __type[100000];
            int __block[100000];
            int __valid[100000];
            int __pointer;
            int __IsInBuffer(std::string table_name,int type,std::string index_name,int block);
            int __IsInBuffer(std::string table_name,int type,int block);
            int __IsInBuffer(std::string table_name,int type);
            int __FindAvailableBufferBlock(std::string table_name);
            std::string __GetFilename(std::string table_name,int type,std::string index_name);
            std::string __GetFilename(std::string table_name,int type);
    };
}
#endif  // DB_DB_BUFFER_H
