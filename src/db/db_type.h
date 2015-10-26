#ifndef DB_DB_TYPE_H
#define DB_DB_TYPE_H

namespace db {
// Syntax type
enum DB_DATA_TYPE {
  TYPE_INT,
  TYPE_CHAR,
  TYPE_FLOAT
};
enum DB_PROPERTY_TYPE {
  TYPE_PRIMARY,
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
private:
  DB_OPERATION_TYPE _op_type;
public:
  virtual int execute() = 0;
};
}  // namespace db

#endif  // DB_DB_TYPE_H
