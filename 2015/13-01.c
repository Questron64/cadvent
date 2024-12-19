#include "../common.c"
#include "../table.c"
#define MAX_GUESTS 100

I solve(I (*map)[MAX_GUESTS][MAX_GUESTS], I *idxs, Z num, Z i) {
  if (i == num - 1) {
    I solution = 0;
    for (Z j = 0; j < num; j++) {
      Z a = idxs[j];
      Z b = idxs[(j + 1) % num];
      solution += (*map)[a][b];
      solution += (*map)[b][a];
    }
    return solution;
  }

  I solution = 0;
  for (Z j = i; j < num; j++) {
    swapi(&idxs[i], &idxs[j]);
    solution = maxi(solution, solve(map, idxs, num, i + 1));
    swapi(&idxs[i], &idxs[j]);
  }
  return solution;
}

I main() {
  Table names = table_init(0, sizeof(Z), 1000);
  Z num_names = 0;
  I(*map)[MAX_GUESTS][MAX_GUESTS] = alloc(0, sizeof(*map));
  I *idxs = seqi(0, MAX_GUESTS);

  S line = {0};
  while (nextline(stdin, &line)) {
    C name[2][32], mode[32];
    I hap;

    I r = sscanf(                                                       //
      line.str,                                                         //
      "%31s would %31s %d happiness units by sitting next to %31[^.].", //
      name[0], mode, &hap, name[1]);                                    //
    if (r != 4)
      die("invalid input");
    if (!strcmp(mode, "lose"))
      hap = -hap;
    else if (strcmp(mode, "gain"))
      die("invalid input");

    Z idx[2] = {0};
    for (I i = 0; i < 2; i++) {
      Z *idx_;
      if (!table_get(&names, name[i], 0, 0, (V **)&idx_)) {
        table_insert(&names, name[i], 0, &num_names, 0, (V **)&idx_);
        num_names++;
      }
      idx[i] = *idx_;
    }

    (*map)[idx[0]][idx[1]] = hap;
  }
  string_reserve(&line, 0);
  table_reserve(&names, 0);

  printf("%d\n", solve(map, idxs, num_names, 0));
  printf("%d\n", solve(map, idxs, num_names + 1, 0));

  idxs = alloc(idxs, 0);
  map = alloc(map, 0);
}
