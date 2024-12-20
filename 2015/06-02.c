#include "common.h"

#include "06-common.c"

I grid[1000][1000];

V turn_on(I x, I y) { grid[y][x]++; }
V turn_off(I x, I y) { grid[y][x] = maxi(grid[y][x] - 1, 0); }
V toggle(I x, I y) { grid[y][x] += 2; }

I main() {
  run(
    1000, 1000, grid,     //
    "turn on", turn_on,   //
    "turn off", turn_off, //
    "toggle", toggle,     //
    NULL);
  printf("%d\n", count_lights(1000, 1000, grid));
}
