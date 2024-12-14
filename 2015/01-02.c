// https://adventofcode.com/2015/day/1
#include "../common.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int floor = 0;
  int position = 0;
  for (int c; c = getchar(), c != EOF;) {
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
