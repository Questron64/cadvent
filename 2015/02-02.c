#include "../common.c"
#include "02-common.c"

int main() {
  Box *boxes;
  size_t numBoxes;
  readBoxes(stdin, &boxes, &numBoxes);

  int ribbon = 0;
  for (size_t i = 0; i < numBoxes; i++) {
    qsort(&boxes[i], 3, sizeof(int), cmpi);
    ribbon += boxes[i][0] * 2 + boxes[i][1] * 2 +
              boxes[i][0] * boxes[i][1] * boxes[i][2];
  }
  printf("%d\n", ribbon);
}
