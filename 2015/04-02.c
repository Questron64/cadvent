#include "../common.c"
#include "../md5.c"
#include "04-common.c"

I main() {
  C *input = readInput(stdin);
  printf("%d\n", findHash(input, "000000"));
  input = alloc(input, 0);
}
