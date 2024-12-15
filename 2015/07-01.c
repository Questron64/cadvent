#include "../common.c"
#include "07-common.c"

I main() {
  loadNet();

  solve('a');
  printf("%" PRIu16 "\n", net['a'].params[0].value);
}
