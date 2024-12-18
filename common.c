// a smorgasbord of utility functions, type names and headers
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// i like my types like i like my Danny DeVitos
typedef bool B;
typedef char C;
typedef const char CC;
typedef unsigned char UC;
typedef unsigned const char UCC;
typedef size_t Z;
typedef const size_t CZ;
typedef int I;
typedef unsigned U;
typedef unsigned int UI;
typedef const int CI;
typedef unsigned const int UCI;
typedef long L;
typedef unsigned long UL;
typedef const long CL;
typedef unsigned const long UCL;
typedef void V;
typedef const void CV;

typedef float F;
typedef double D;

typedef int8_t I8;
typedef uint8_t U8;
typedef int16_t I16;
typedef uint16_t U16;
typedef int32_t I32;
typedef uint32_t U32;
typedef int64_t I64;
typedef uint64_t U64;

// print message to console and kill the program
_Noreturn V die(CC *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}

// one alloc function to rule them and
// in the darkness bind them
//
// dies on allocation error
//
// note that allocated memory is zeroes, but realloced
// memory is not
//
// to malloc call foo = alloc(0, size)
// to realloc call foo = alloc(foo, new_size)
// to free call foo = alloc(foo, 0)
V *alloc(V *ptr, Z size) {
  if (size == 0) { // free
    free(ptr);
    return NULL;
  } else if (ptr == NULL) { // calloc
    V *new = calloc(1, size);
    if (!new)
      die("memory error");
    return new;
  } else { // realloc
    V *new = realloc(ptr, size);
    if (!new)
      die("memory error");
    return new;
  }
}

// get the number of bits set in x
I popcount(U x) {
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_popcount(x);
#else
  I count = 0;
  for (U m = 1; m; m <<= 1)
    count += !!(x & m);
  return count;
#endif
}

// get the number of characters in the ASCII representation of i
I digits(I i) {
  I d;
  if (i == 0)
    d = 1;
  else
    d = (I)floor(log10(abs(i))) + 1;
  if (i < 0)
    d++;
  return d;
}

// return a new array of integers from a to b exclusive
// a must be < b
I *seqi(I a, I b) {
  I *arr = alloc(0, sizeof(I) * (b - a));
  for (I i = a; i < b; i++)
    arr[i] = a + i;
  return arr;
}

// return true if string starts with prefix
B startswith(CC *string, CC *prefix) {
  return !strncmp(string, prefix, strlen(prefix));
}

// return true if string starts with any prefix passed
// list must be null-terminated, like
// if(startswith_any(foo, "bar", "baz", 0))
B startswith_any(CC *string, ...) {
  va_list args;
  va_start(args, string);
  for (CC *prefix; prefix = va_arg(args, CC *), prefix;) {
    if (startswith(string, prefix)) {
      va_end(args);
      return true;
    }
  }
  va_end(args);
  return false;
}

// returns non-zero if c is a vowel
I isvowel(I c) { return !!strchr("aeiouAEIOU", c); }

// return the small of two integers
I mini(I a, I b) {
  if (a < b)
    return a;
  return b;
}

// return the larger of two integers
I maxi(I a, I b) {
  if (a > b)
    return a;
  return b;
}

// return the sign of an integer
// -1 if a < 0
// 0 if a == 0
// 1 if a > 0
I signi(I a) {
  if (a < 0)
    return -1;
  else if (a > 0)
    return 1;
  return 0;
}

// swap two integers
V swapi(I *a, I *b) {
  I c = *a;
  *a = *b;
  *b = c;
}

// compare two integers
// mostly for qsort
I cmpi(CV *a, CV *b) {
  CI *a_ = a;
  CI *b_ = b;
  return *a_ - *b_;
}

// get a line of input from a file
// reads complete lines and allocates as necessary
// returns the length of the line read
//
// swiss army knife input function
//
// C *line = NULL;
// Z line_size = 0;
// while(fgetline(stdin, &line, &line_size)) {
//   // ...
// }
Z fgetline(FILE *f, C **buf, Z *buf_size) {
  Z dummy_buf_size = 0;
  if (!buf_size)
    buf_size = &dummy_buf_size;

  if (*buf_size == 0) {
    *buf_size = 512;
    *buf = alloc(*buf, *buf_size);
  }

  Z idx = 0;
  while (1) {
    C *result = fgets(*buf + idx, *buf_size - idx, f);
    if (!result)
      return idx;

    idx += strlen(*buf + idx);
    if (idx == 0) {
      // we somehow read zero bytes
      // this should not happen, but we can't index
      // (*buf)[idx - 1] to check for newline, return 0
      return idx;
    }

    if ((*buf)[idx - 1] == '\r' || (*buf)[idx - 1] == '\n')
      return idx;

    // we read some characters, but not a newline
    // if we're at the end of the buffer then we
    // should resize it and try again
    if (idx == *buf_size - 1) {
      *buf_size *= 2;
      *buf = alloc(*buf, *buf_size);
    }
  }
}

Z fgetfile(FILE *f, C **buf, Z *buf_size) {
  if(*buf_size == 0) {
    *buf_size = 512;
    *buf = alloc(*buf, *buf_size);
  }

  Z idx = 0;
  while(!feof(f)) {
    if (idx == *buf_size) {
      *buf_size *= 2;
      *buf = alloc(*buf, *buf_size);
    }

    idx += fread(&(*buf)[idx], 1, *buf_size - idx, f);
  }

  return idx;
}

// remove whitespace from the end of a string
// return new length of string
Z trim(C *str) {
  Z i = strlen(str);
  while (i > 1 && isspace(str[i - 1])) {
    str[i - 1] = 0;
    i--;
  }
  return i;
}
