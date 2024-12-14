#include "../common.c"
#include "../grid.c"

I main() {
  Grid g = {0};
  I pos[2][2] = {0};
  Z idx = 0;
  I numHouses = 1;
  (*indexGrid(&g, 0, 0))++;

  for (I c; c = fgetc(stdin), c != EOF;) {
    switch (c) {
    case '<': pos[idx][0]--; break;
    case '>': pos[idx][0]++; break;
    case '^': pos[idx][1]--; break;
    case 'v': pos[idx][1]++; break;
    case ' ':
    case '\r':
    case '\n': break;
    default: die("input error");
    }

    I *house = indexGrid(&g, pos[idx][0], pos[idx][1]);
    if (!*house)
      numHouses++;
    (*house)++;

    idx ^= 1;
  }

  printf("%d\n", numHouses);
  g = freeGrid(&g);
}

