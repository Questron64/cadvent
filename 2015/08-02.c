#include "common.h"

I main() {
  C *line = NULL;
  Z line_size = 0;

  I original = 0;
  I encoded = 0;
  while (fgetline(stdin, &line, &line_size)) {
    original += trim(line);
    encoded += 2; // quotation marks

    for (C *c = line; *c; c++) {
      if (*c == '"' || *c == '\\')
        encoded++;
      encoded++;
    }
  }

  line = alloc(line, 0);
  printf("%d\n", encoded - original);
}
