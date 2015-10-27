#include "db/db_repl.h"

#include <iostream>
#include <string>
using namespace std;

#include "base/string.h"
#include "db/db_process.h"

namespace db {
int DBREPL(istream& in, bool prompt) {
  cout << "MiniSQL shell version: 0.0.1" << endl;
  cout << "connecting to: default" << endl;
  if (prompt) {
    cout << "> ";
  }
  string chunk = "", buffer = "", command = "";
  while (getline(in, chunk)) {
    if (buffer.length() > 0)
      buffer += " ";
    buffer = buffer + chunk;
    size_t pos;
    while ((pos = buffer.find(';')) != string::npos) {
      command = buffer.substr(0, pos + 1);
      buffer = buffer.substr(pos + 1);
      if (DBProcess(base::String::Trim(command))) {
        break;
      }
    }
    if (prompt) {
      if (buffer.length() > 0) {
        cout << "... ";
      } else {
        cout << "> ";
      }
    }
  }
  if (prompt) {
    cout << endl << "bye" << endl;
  }
  return 0;
}
}  // namespace db
