// A smorgasbord of utility functions, type names and headers
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char C;
typedef const char CC;
typedef size_t Z;
typedef const size_t CZ;
typedef int I;
typedef const int CI;
typedef void V;
typedef const void CV;

_Noreturn V die(CC *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

V *alloc(V *ptr, Z size) {
  if (size == 0) {
    free(ptr);
    return NULL;
  } else if (ptr == NULL) {
    V *new = calloc(1, size);
    if (!new)
      die("memory error");
    return new;
  } else {
    V *new = realloc(ptr, size);
    if (!new)
      die("memory error");
    return new;
  }
}

int mini(int a, int b) {
  if (a < b)
    return a;
  return b;
}

V swapi(int *a, int *b) {
  int c = *a;
  *a = *b;
  *b = c;
}

int cmpi(CV *a, CV *b) {
  CI *a_ = a;
  CI *b_ = b;
  return *a_ - *b_;
}

Z fgetline(FILE *f, C **buf, Z *bufSize) {
  Z dummyBufSize = 0;
  if(!bufSize)
    bufSize = &dummyBufSize;

  if (*bufSize == 0) {
    *bufSize = 512;
    *buf = alloc(*buf, *bufSize);
  }

  Z idx = 0;
  while (1) {
    C *result = fgets(*buf + idx, *bufSize - idx, f);
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
      *buf = alloc(*buf, *bufSize);
    }
  }
}

Z trim(C *str) {
  Z i = strlen(str);
  while(i > 1 && isspace(str[i - 1])) {
    str[i - 1] = 0;
    i--;
  }
  return i;
}
