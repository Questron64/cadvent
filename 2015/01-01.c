#include "../common.c"

I main(I argc, C *argv[]) {
  I floor = 0;
  for (I c; c = getchar(), c != EOF;) {
    switch (c) {
    case '(': floor++; break;
    case ')': floor--; break;
    default: die("invalid input");
    }
  }
  printf("%d\n", floor);
}
