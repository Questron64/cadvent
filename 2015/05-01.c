#include "../common.c"

int main() {
  C *line = NULL;
  Z line_size = 0;

  I nice = 0;
  while (fgetline(stdin, &line, &line_size)) {
    trim(line);

    I vowels = 0;
    I repeat = 0;
    B forbidden = false;

    for (Z i = 0, len = strlen(line); i < len; i++) {
      if (isvowel(line[i]))
        vowels++;
      if (line[i] == line[i + 1])
        repeat = 1;
      if (startswith_any(&line[i], "ab", "cd", "pq", "xy", 0))
        forbidden = true;
    }

    if (vowels >= 3 && repeat && !forbidden)
      nice++;
  }

  printf("%d\n", nice);
  line = alloc(line, 0);
}
