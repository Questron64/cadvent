#include "../common.c"
#include "../json.c"
#include "../table.c"

V accumulate_number(C *num, Z num_len, V *user) {
  C *buf = alloc(0, num_len + 1);
  strncpy(buf, num, num_len);
  buf[num_len] = 0;

  C *end = NULL;
  errno = 0;
  D n = strtod(buf, &end);
  if (errno == ERANGE || end != &buf[num_len])
    die("invalid number");

  D *acc = user;
  *acc += n;

  buf = alloc(buf, 0);
}

I main() {
  Z buf_size = 0;
  C *buf = NULL;
  Z file_len = fgetfile(stdin, &buf, &buf_size);
  if (!file_len)
    die("input error");

  D acc = 0;

  JSONParserState p = {
    .buf = buf,
    .buf_len = file_len,
    .idx = 0,

    .user = &acc,
    .on_number = accumulate_number,
  };

  if (!json_json(&p))
    die("invalid json");
  printf("%f\n", acc);
}
