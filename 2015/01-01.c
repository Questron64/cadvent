// https://adventofcode.com/2015/day/1
#include "../common.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int floor = 0;
  for (int c; c = getchar(), c != EOF;) {
    switch (c) {
    case '(': floor++; break;
    case ')': floor--; break;
    default: die("invalid input");
    }
  }
  printf ("%d\n", floor);
}
