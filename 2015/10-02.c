#include "../common.c"
#include "10-common.c"

I main() {
  C *line = NULL;
  if (!fgetline(stdin, &line, 0))
    die("input error");
  trim(line);

  printf("%d\n", look_and_say(line, 0, 50));
}
