#include "../common.c"
#include "../md5.c"
#include "04-common.c"

I main() {
  C *input = read_input(stdin);
  printf("%d\n", find_hash(input, "000000"));
  input = alloc(input, 0);
}
