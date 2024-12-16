#include "../common.c"
#include "../table.c"

I main() {
  Table t = table_init(sizeof(I), sizeof(I));
  table_reserve(&t, 1000);

  for (I i = 0; i < 11; i++)
    table_insert(&t, &i, &(int){i + 7});
  table_delete_by_key(&t, &(int){3});

  for (Z idx = table_start(&t);   //
       idx != table_end(&t);      //
       idx = table_next(&t, idx)) //
  {
    I *k, *v;
    table_get_by_idx(&t, idx, (CV**)&k, (V**)&v);
    printf("%d = %d\n", *k, *v);
  }

  table_reserve(&t, 0);
}
