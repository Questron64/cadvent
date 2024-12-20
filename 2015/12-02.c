#include "common.h"
#include "json.h"
#include "table.h"

D accumulate(JSONValue *v, D n) {
  switch (v->type) {
  case JSON_NUMBER: n += v->number; break;
  case JSON_ARRAY:
    for (Z i = 0; i < v->array.size; i++)
      n = accumulate(&v->array.values[i], n);
    break;
  case JSON_OBJECT: {
    D back = n;
    for (Z i = table_start(&v->object); //
         i != table_end(&v->object);    //
         i = table_next(&v->object, i)  //
    ) {
      JSONValue *v2;
      table_get_by_idx(&v->object, i, 0, (V **)&v2);
      if (v2->type == JSON_STRING && !strcmp(v2->string, "red")) {
        n = back;
        break;
      } else {
        n = accumulate(v2, n);
      }
    }
  } break;
  default:
  }
  return n;
}

I main() {
  JSONValue root = json_parse_file(stdin);
  if (root.type == JSON_INVALID)
    die("invalid json");
  printf("%f\n", accumulate(&root, 0));
  json_free(&root);
}
