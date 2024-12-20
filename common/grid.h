#ifndef GRID_H
#define GRID_H
#include "common.h"

typedef struct {
  Z size;
  V *data;
} Grid;

Grid free_grid(Grid *this);
I *index_grid(Grid *this, I x, I y);

#endif