#include "base/io.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
using namespace std;

namespace base {
namespace IO {
/* Read a string and return it.  Returns false on EOF, otherwise true. */
bool ReadLine(string& dest, string prompt) {
  char* line_read = (char *)NULL;
  /* Get a line from the user. */
  line_read = readline(prompt.c_str());
  /* EOF */
  if (line_read == NULL)
    return false;
  /* If the line has any text in it, save it on the history. */
  if (*line_read)
    add_history(line_read);
  /* Save string. */
  dest = line_read;
  free(line_read);
  return true;
}
int InitializeHistory() {
  return read_history(NULL);
}
int SaveHistory() {
  return write_history(NULL);
}
}  // namespace IO
}  // namespace base
