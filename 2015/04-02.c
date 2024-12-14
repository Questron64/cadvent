#include "../common.c"
#include "../md5.c"
#include "04-common.c"

I main() {
  C *input = NULL;
  Z inputSize = 0;

  if (fgetline(stdin, &input, &inputSize) == 0)
    die("input error");
  inputSize = trim(input);

  printf("%d\n", findHash(input, "000000"));

  input = alloc(input, 0);
}
