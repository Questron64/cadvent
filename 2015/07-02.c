#include "common.h"

#include "07-common.c"

I main() {
  load_net();
  net['b'] = (Node){VALUE, {{NUMBER, 46065}, {NUMBER, 0}}};

  solve('a');
  printf("%" PRIu16 "\n", net['a'].params[0].value);
}
