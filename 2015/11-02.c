#include "../common.c"
#include "11-common.c"

I main() {
  C *pass = NULL;
  if (!fgetline(stdin, &pass, 0))
    die("input error");
  Z pass_len = trim(pass);

  for (I i = 0; i < 2; i++) {
    do {
      increment_password(pass_len, pass, pass_len - 1);
    } while (!is_password_valid(pass_len, pass));
  }
  printf("%s\n", pass);
}
