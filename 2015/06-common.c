V run(Z w, Z h, I grid[h][w], ...) {
  C *line = NULL;
  Z lineSize = 0;
  while (fgetline(stdin, &line, &lineSize)) {
    va_list args;
    va_start(args, grid);

    CC *cmd;
    while (cmd = va_arg(args, CC *), cmd) {
      V (*func)(I, I) = va_arg(args, V(*)(I, I));

      if (!startswith(line, cmd))
        continue;

      I x1, y1, x2, y2;
      if (
        sscanf(
          line + strlen(cmd),              //
          "%d,%d through %d,%d", &x1, &y1, //
          &x2, &y2) != 4                   //
      ) {
        die("input error");
      }

      if (x1 == x2 || y1 == y2)
        die("input error");

      for (I y = y1, yd = signi(y2 - y1); y <= y2; y += yd) {
        for (I x = x1, xd = signi(x2 - x1); x <= x2; x += xd) {
          if (x1 < 0 || x1 >= w || x2 < 0 || x2 >= w ||
              y1 < 0 || y1 >= h || y2 < 0 || y2 >= h) {
            die("range error");
          }
          func(x, y);
        }
      }

      break;
    }
    if (!cmd)
      die("input error");

    va_end(args);
  }
}

I count_lights(I w, I h, I grid[h][w]) {
  I lights = 0;
  for (Z y = 0; y < 1000; y++)
    for (Z x = 0; x < 1000; x++)
      lights += grid[y][x];
  return lights;
}
