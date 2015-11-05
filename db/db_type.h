#ifndef DB_DB_TYPE_H
#define DB_DB_TYPE_H

#include <string>
#include <vector>

namespace db {

// COMMAND
enum COMMAND {
  COMMAND_QUIT = 0xDEADBEEF
};

// Error type
enum ERROR_CODE {
  SYNTAX_ERROR = 1,
  RUNTIME_ERROR,
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
  TYPE_UNIQUE,
  TYPE_INDEXED,
  TYPE_PRIMARY_KEY
};
enum DB_OPERATION_TYPE {
  TYPE_CREATE_TABLE,
  TYPE_DROP_TABLE,
  TYPE_CREATE_INDEX,
  TYPE_DROP_INDEX,
  TYPE_SELECT_FROM,
  TYPE_INSERT_INTO,
  TYPE_DELETE_FROM,
  TYPE_QUIT,
  TYPE_EXECUTE
};

// Relations
enum RELATION {
  LT, LTE, GT, GTE, NEQ, EQ
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

// Filter
class Filter {
public:
  std::string key;
  RELATION op;
  std::string value;
  Filter(std::string s);
};

// CREATE TABLE Operation
class CreateTableOperation: public Operation {
private:
  std::string table_name;
  std::vector<Attribute> attr_list;
public:
  CreateTableOperation(std::string command);
  int Execute();
};

// DROP TABLE Operation
class DropTableOperation: public Operation {
private:
  std::string table_name;
public:
  DropTableOperation(std::string command);
  int Execute();
};

// CREATE INDEX Operation
class CreateIndexOperation: public Operation {
private:
  std::string index_name, table_name, attr_name;
public:
  CreateIndexOperation(std::string command);
  int Execute();
};

// DROP INDEX Operation
class DropIndexOperation: public Operation {
private:
  std::string index_name;
public:
  DropIndexOperation(std::string command);
  int Execute();
};

// INSERT INTO Operation
class InsertIntoOperation: public Operation {
private:
  std::string table_name;
  std::vector<std::string> values;
public:
  InsertIntoOperation(std::string command);
  int Execute();
};

// SELECT FROM Operation
class SelectFromOperation: public Operation {
private:
  std::string table_name;
  std::vector<Filter> filters;
public:
  SelectFromOperation(std::string command);
  int Execute();
};

// DELETE FROM Operation
class DeleteFromOperation: public Operation {
private:
  std::string table_name;
  std::vector<Filter> filters;
public:
  DeleteFromOperation(std::string command);
  int Execute();
};

// EXECFILE Operation
class ExecfileOperation: public Operation {
private:
  std::string filepath;
public:
  ExecfileOperation(std::string command);
  int Execute();
};
}  // namespace db
#endif  // DB_DB_TYPE_H
