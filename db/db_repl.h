#ifndef DB_DB_REPL_H
#define DB_DB_REPL_H

#include <iostream>

namespace db {
int DBREPL(std::istream& in, bool prompt = true, bool module = false);
}  // namespace db

#endif  // DB_DB_REPL_H
