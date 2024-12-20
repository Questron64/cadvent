#include "common.h"

#include "02-common.c"

I main() {
  Box *boxes;
  Z num_boxes;
  read_boxes(stdin, &boxes, &num_boxes);

  I paper = 0;
  for (Z i = 0; i < num_boxes; i++) {
    qsort(&boxes[i], 3, sizeof(I), cmpi);
    paper += boxes[i][0] * boxes[i][1] * 3 + //
             boxes[i][1] * boxes[i][2] * 2 + //
             boxes[i][2] * boxes[i][0] * 2;  //
  }
  printf("%d\n", paper);

  boxes = alloc(boxes, 0);
}
