#include "common.h"
#include "md5.h"

#include "04-common.c"

I main() {
  C *input = read_input(stdin);
  printf("%d\n", find_hash(input, "000000"));
  input = alloc(input, 0);
}
