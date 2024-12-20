#include "common.h"

#include "09-common.c"

I main() {
  I num_cities = read_input();
  I longest = 0;
  I *idxs = seqi(0, num_cities);

  solve(num_cities, idxs, 0, 0, 0, &longest);
  printf("%d\n", longest);

  idxs = alloc(idxs, 0);
}
