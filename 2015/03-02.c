#include "common.h"
#include "grid.h"

I main() {
  Grid g = {0};
  I pos[2][2] = {0};
  Z idx = 0;
  I num_houses = 1;
  (*index_grid(&g, 0, 0))++;

  for (I c; c = fgetc(stdin), c != EOF;) {
    switch (c) {
    case '<': pos[idx][0]--; break;
    case '>': pos[idx][0]++; break;
    case '^': pos[idx][1]--; break;
    case 'v': pos[idx][1]++; break;
    case ' ':
    case '\r':
    case '\n': break;
    default: die("input error");
    }

    I *house = index_grid(&g, pos[idx][0], pos[idx][1]);
    if (!*house)
      num_houses++;
    (*house)++;

    idx ^= 1;
  }

  printf("%d\n", num_houses);
  g = free_grid(&g);
}
