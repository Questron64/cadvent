#ifndef COMMON_H
#define COMMON_H
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

typedef struct {
  Z len;  // length of the string
  Z cap;  // capacity of the buffer
  C *str; // pointer to first char (may be NULL if cap or len is 0)
} String, S;

_Noreturn V die(CC *fmt, ...);
V *alloc(V *ptr, Z size);
I popcount(U x);
I digits(I i);
I *seqi(I a, I b);
B startswith(CC *string, CC *prefix);
B startswith_any(CC *string, ...);
I isvowel(I c);
I mini(I a, I b);
I maxi(I a, I b);
I signi(I a);
V swapi(I *a, I *b);
I cmpi(CV *a, CV *b);
B nextline(FILE *f, S *s);
Z fgetline(FILE *f, C **buf, Z *buf_size);
Z fgetfile(FILE *f, C **buf, Z *buf_size);
Z trim(C *str);
V string_trim(S *s);
V string_reserve(S *s, Z cap);

#endif