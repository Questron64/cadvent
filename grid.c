// An infinitely-expanding grid of integers
typedef struct {
  Z size;
  V *data;
} Grid;

Grid freeGrid(Grid *this) {
  if (this) {
    alloc(this->data, 0);
  }
  return (Grid){0};
}

I *indexGrid(Grid *this, I x, I y) {
  I rx = (I)this->size / 2 + x;
  I ry = (I)this->size / 2 + y;

  // If the requested cell is not on the current grid then
  // we need to resize the grid and copy the data over
  {
    Z newSize = this->size;
    while (rx < 0 || ry < 0 ||                  //
           rx >= (I)newSize || ry >= (I)newSize //
    ) {
      if (newSize == 0) {
        newSize = 128;
        rx = newSize / 2;
        ry = newSize / 2;
      } else {
        newSize = newSize * 2;
        rx += newSize / 4;
        ry += newSize / 4;
      }
    }

    if (newSize != this->size) {
      // clang-format really hates this line
      I(*n) // it doesn't know about pointer-to-array
      [newSize][newSize] =
        alloc(NULL, sizeof(I[newSize][newSize]));

      if (this->size != 0) {
        I(*t)[this->size][this->size] = this->data;
        Z off = (newSize - this->size) / 2;
        for (Z y = 0; y < this->size; y++) {
          memcpy(                //
            &(*n)[off + y][off], //
            &(*t)[y][0],         //
            sizeof((*t)[y]));    //
        }
      }

      this->data = alloc(this->data, 0);
      this->data = n;
      this->size = newSize;
    }
  }

  // Index the grid
  I(*g)[this->size][this->size] = this->data;
  return &(*g)[ry][rx];
}
