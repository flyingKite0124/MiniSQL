//
// Created by GreyTone on 15-11-4.
//

#include "db/db_index.h"

bool db::CreateIndex(std::string table_name, std::string index_name) {
    BufferDelegate.CreateIndex(table_name, index_name);
}

bool db::DropIndex(std::string table_name, std::string index_name) {
    BufferDelegate.DropIndex(table_name, index_name);
}

bool db::RecreateIndex(std::string table_name, std::string index_name) {
    BufferDelegate.DropIndex(table_name, index_name);
    BufferDelegate.CreateIndex(table_name, index_name);
}
