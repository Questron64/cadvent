#include "../common.c"
#include "09-common.c"

I main() {
  I num_cities = read_input();
  I shortest = INT_MAX;
  I *idxs = seqi(0, num_cities);

  solve(num_cities, idxs, 0, 0, &shortest, 0);
  printf("%d\n", shortest);

  idxs = alloc(idxs, 0);
}
