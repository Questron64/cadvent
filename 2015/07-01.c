#include "../common.c"
#include "07-common.c"

I main() {
  load_net();

  solve('a');
  printf("%" PRIu16 "\n", net['a'].params[0].value);
}
