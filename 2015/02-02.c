#include "../common.c"
#include "02-common.c"

I main() {
  Box *boxes;
  Z numBoxes;
  readBoxes(stdin, &boxes, &numBoxes);

  I ribbon = 0;
  for (Z i = 0; i < numBoxes; i++) {
    qsort(&boxes[i], 3, sizeof(I), cmpi);
    ribbon += boxes[i][0] * 2 + boxes[i][1] * 2 +
              boxes[i][0] * boxes[i][1] * boxes[i][2];
  }
  printf("%d\n", ribbon);

  boxes = alloc(boxes, 0);
}
