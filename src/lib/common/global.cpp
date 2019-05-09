//
// Created by victor on 19-3-5.
//

#include "global.h"

namespace vzc {

std::string StringFormat(char const *format, ...) {
  va_list args, args_copy;
  va_start(args, format);
  va_copy(args_copy, args);

  int len = vsnprintf(nullptr, 0, format, args_copy);
  char *buffer = new char[len + 1];
  memset(buffer, 0, len + 1);
  va_end(args_copy);

  vsnprintf(buffer, len + 1, format, args);
  va_end(args);

  std::string ret(buffer);
  delete[] buffer;
  buffer = nullptr;
  return ret;
}

}
