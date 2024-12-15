#include "../common.c"

I main() {
  C *line = NULL;
  Z line_size = 0;

  I encoded = 0;
  I unencoded = 0;
  while (fgetline(stdin, &line, &line_size)) {
    unencoded += trim(line);
    C *c = line;

    while (isspace(*c))
      c++;
    if (*c != '"')
      die("input error");
    c++;

    for (; *c && *c != '"';) {
      if (startswith(c, "\\\\") || startswith(c, "\\\""))
        c++;
      else if (startswith(c, "\\x"))
        c += 3;
      encoded++;
      c++;
    }

    if (*c != '"')
      die("input error");
  }

  line = alloc(line, 0);
  printf("%d\n", unencoded - encoded);
}
