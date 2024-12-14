#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Noreturn void die(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

int mini(int a, int b) {
  if (a < b)
    return a;
  return b;
}

void swapi(int *a, int *b) {
  int c = *a;
  *a = *b;
  *b = c;
}

int cmpi(const void *a, const void *b) {
  const int *a_ = a;
  const int *b_ = b;
  return *a_ - *b_;
}

size_t fgetline(FILE *f, char **buf, size_t *bufSize) {
  if (*bufSize == 0) {
    *bufSize = 512;
    *buf = malloc(*bufSize);
    if (!*buf)
      die("memory error");
  }

  size_t idx = 0;
  while (1) {
    char *result = fgets(*buf + idx, *bufSize - idx, f);
    if (!result)
      return idx;

    idx += strlen(*buf + idx);
    if (idx == 0) {
      // We somehow read zero bytes. This should not happen,
      // but we can't index (*buf)[idx - 1] to check for newline
      // so we return 0
      return idx;
    }

    if ((*buf)[idx - 1] == '\r' || (*buf)[idx - 1] == '\n')
      return idx;

    // We read some characters, but not a newline. If we're at
    // the end of the buffer then we should resize it and try
    // again
    if (idx == *bufSize - 1) {
      *bufSize *= 2;
      char *new = realloc(*buf, *bufSize);
      if (!new)
        die("memory error");
      *buf = new;
    }
  }
}
