#ifndef DB_DB_REPL_H
#define DB_DB_REPL_H

#include <iostream>

namespace db {
int DBREPL(bool module);
int DBREPL(bool module, std::istream& in);
}  // namespace db

#endif  // DB_DB_REPL_H
