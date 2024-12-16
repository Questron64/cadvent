#include "../table.c"
#define MAX_CITIES 20

I map[MAX_CITIES][MAX_CITIES] = {0};

V solve(
  I num_cities, I idxs[num_cities], //
  I n, I dist,                      //
  I *shortest, I *longest)          //
{
  if (n == num_cities) {
    if (shortest && dist < *shortest)
      *shortest = dist;
    if (longest && dist > *longest)
      *longest = dist;
    return;
  }

  for (I i = n; i < num_cities; i++) {
    swapi(&idxs[i], &idxs[n]);

    I d = 0;
    if (n > 0)
      d = map[idxs[n]][idxs[n - 1]];

    dist += d;
    solve(num_cities, idxs, n + 1, dist, shortest, longest);
    dist -= d;

    swapi(&idxs[i], &idxs[n]);
  }
}

// read input into map, return number of cities
I read_input() {
  I num_cities = 0;

  Table index = table_init(0, sizeof(I), 1000);

  C *line = NULL;
  Z line_size = 0;
  while (fgetline(stdin, &line, &line_size)) {
    C from[32], to[32];
    I distance;
    if (sscanf(line, "%31s to %31s = %d", from, to, &distance) != 3)
      die("input error");

    I *from_idx;
    if (table_insert(&index, from, &num_cities, 0, (V **)&from_idx))
      num_cities++;

    I *to_idx;
    if (table_insert(&index, to, &num_cities, 0, (V **)&to_idx))
      num_cities++;

    // bounds check on map, should never happen
    if (num_cities >= MAX_CITIES)
      die("too many cities");

    map[*from_idx][*to_idx] = distance;
    map[*to_idx][*from_idx] = distance;
  }

  table_reserve(&index, 0);

  return num_cities;
}
