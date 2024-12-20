#include "common.h"

I main(I argc, C *argv[]) {
  I floor = 0;
  I position = 0;
  for (I c; c = getchar(), c != EOF;) {
    position++;
    switch (c) {
    case '(': floor++; break;
    case ')': floor--; break;
    default: die("invalid input");
    }
    if (floor < 0)
      break;
  }
  printf("%d\n", position);
}
