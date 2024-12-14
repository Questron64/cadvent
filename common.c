#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

_Noreturn void die(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(EXIT_FAILURE);
}
