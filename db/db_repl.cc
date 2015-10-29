#include "db/db_repl.h"

#include <iostream>
#include <string>
using namespace std;

#include "base/string.h"
#include "db/db_type.h"
#include "db/db_process.h"

namespace db {
int DBREPL(istream& in, bool prompt, bool module) {
  if (!module) {
    cout << "MiniSQL shell version: 0.0.1" << endl;
    cout << "connecting to: default" << endl;
  }
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
      command = buffer.substr(0, pos);
      buffer = buffer.substr(pos + 1);
      int ret_code = DBProcess(base::String::Trim(command));
      if (ret_code == COMMAND_QUIT) {
        goto Quit;
      } else if (ret_code != 0) {
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
Quit:
  if (prompt) {
    cout << endl << "bye" << endl;
  }
  return 0;
}
}  // namespace db
