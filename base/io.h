#ifndef BASE_IO_H
#define BASE_IO_H
#include <string>

namespace base {
namespace IO {
bool ReadLine(std::string& dest, std::string prompt);
int InitializeHistory();
int SaveHistory();
}  // namespace IO
}  // namespace base
#endif  // BASE_IO_H
