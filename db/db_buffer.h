#ifndef DB_DB_BUFFER_H
#define DB_DB_BUFFER_H 

#include <string>
namespace db{
    const int CATALOG = 0; 
    const int INDEX = 1; 
    const int DATA = 2; 
    const int INDEXPAGE = 3; 
    const int DATAPAGE = 4; 

    class Buffer
    {
        public:   
            Buffer();
            ~Buffer();
            int CreateTable(std::string table_name);
            int DropTable(std::string table_name);
            int CheckTable(std::string table_name);

            int ReadCatalogBlock(std::string table_name,std::string &content);
            int WriteCatalogBlock(std::string table_name,const std::string content);

            int CreateIndex(std::string table_name,std::string index);
            int DropIndex(std::string table_name,std::string index);

            int GetRootNumber(std::string table_name,std::string index_name);
            int SetRootNumber(std::string table_name,std::string index_name,int block);
            int ReadIndexBlock(std::string table_name,std::string index_name,int block,std::string &content);
            int WriteIndexBlock(std::string table_name,std::string index_name,int block,const std::string content);

            int GetEmptyIndexBlock(std::string table_name,std::string index_name);
            int SetUsedIndexBlock(std::string table_name,std::string index_name,int block);
            int DeleteIndexBlock(std::string table_name,std::string index_name,int block);

            int ReadDataBlock(std::string table_name,int block,std::string &content);
            int WriteDataBlock(std::string table_name,int block,const std::string content);

            int GetAvailableDataBlock(std::string table_name);
            int SetFullDataBlock(std::string table_name,int block);
            int SetAvailableDataBlock(std::string table_name,int block);
        private:
            unsigned char space[100][4000];
            std::string filename[100]; 
            int block[100];
            int valid[100];
            int __IsInBuffer(std::string filename,int block);
            std::string __GetFilename(std::string table_name,int type,std::string index);
    };
}
#endif 


