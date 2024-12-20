#include "common.h"
#include "grid.h"

I main() {
  Grid g = {0};
  I x = 0, y = 0;
  I num_houses = 1;
  (*index_grid(&g, 0, 0))++;

  for (I c; c = fgetc(stdin), c != EOF;) {
    switch (c) {
    case '<': x--; break;
    case '>': x++; break;
    case '^': y--; break;
    case 'v': y++; break;
    case ' ':
    case '\r':
    case '\n': break;
    default: die("input error");
    }

    I *house = index_grid(&g, x, y);
    if (!*house)
      num_houses++;
    (*house)++;
  }

  printf("%d\n", num_houses);
  g = free_grid(&g);
}
