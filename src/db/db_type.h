#ifndef DB_DB_TYPE_H
#define DB_DB_TYPE_H

#include <string>
#include <vector>

namespace db {

// Error type
enum ERROR_CODE {
  SYNTAX_ERROR = 1,
  UNKNOWN_ERROR
};

// Syntax type
enum DB_DATA_TYPE {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_FLOAT
};
enum DB_ATTRIBUTE_TYPE {
  TYPE_NONE,
  TYPE_PRIMARY_KEY,
  TYPE_UNIQUE
};
enum DB_OPERATION_TYPE {
  TYPE_CREATE_TABLE,
  TYPE_DROP_TABLE,
  TYPE_CREATE_INDEX,
  TYPE_DROP_INDEX,
  TYPE_SELECT,
  TYPE_INSERT,
  TYPE_DELETE,
  TYPE_QUIT,
  TYPE_EXECUTE
};

// Operations' basic class
class Operation {
protected:
  DB_OPERATION_TYPE op_type;
public:
  virtual int Execute() = 0;
};

// AttributeType
typedef struct {
  std::string name;
  DB_DATA_TYPE type;
  size_t size;
  DB_ATTRIBUTE_TYPE attribute_type;
} Attribute;

// CREATE TABLE Operation
class CreateTable : public Operation {
private:
  std::string table_name;
  std::vector<Attribute> attr_list;
public:
  CreateTable(std::string command);
  int Execute();
};

// DROP TABLE Operation
class DropTable : public Operation {
private:
  std::string table_name;
public:
  DropTable(std::string command);
  int Execute();
};

// CREATE INDEX Operation
class CreateIndex : public Operation {
private:
  std::string index_name, table_name, attr_name;
public:
  CreateIndex(std::string command);
  int Execute();
};
}  // namespace db
#endif  // DB_DB_TYPE_H
