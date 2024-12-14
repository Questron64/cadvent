#include "../common.c"
#include "../grid.c"

I main() {
  Grid g = {0};
  I x = 0, y = 0;
  I numHouses = 1;
  (*indexGrid(&g, 0, 0))++;

  for (I c; c = fgetc(stdin), c != EOF;) {
    switch (c) {
    case '<': x--; break;
    case '>': x++; break;
    case '^': y--; break;
    case 'v': y++; break;
    case ' ':
    case '\r':
    case '\n': break;
    default: die("input error");
    }

    I *house = indexGrid(&g, x, y);
    if (!*house)
      numHouses++;
    (*house)++;
  }

  printf("%d\n", numHouses);
  g = freeGrid(&g);
}
