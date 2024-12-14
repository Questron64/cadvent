#include "../common.c"

int main() {
  C *line = NULL;
  Z lineSize = 0;

  I nice = 0;
  while (fgetline(stdin, &line, &lineSize)) {
    Z len = trim(line);

    B pair = 0;
    B repeat = 0;

    for (Z i = 1; !pair && i < len - 2; i++)
      for(Z j = i + 2; !pair && j < len; j++)
        if(line[j] == line[i] && line[j - 1] == line[i - 1])
          pair = true;

    for(Z i = 2; pair && !repeat && i < len; i++)
      if(line[i] == line[i - 2])
        repeat = true;

    if(pair && repeat)
      nice++;
  }

  printf("%d\n", nice);
  line = alloc(line, 0);
}
