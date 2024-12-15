// an infinitely-expanding grid of integers
typedef struct {
  Z size;
  V *data;
} Grid;

Grid free_grid(Grid *this) {
  if (this) {
    alloc(this->data, 0);
  }
  return (Grid){0};
}

I *index_grid(Grid *this, I x, I y) {
  I rx = (I)this->size / 2 + x;
  I ry = (I)this->size / 2 + y;

  // if the requested cell is not on the current grid then
  // we need to resize the grid and copy the data over
  {
    Z new_size = this->size;
    while (rx < 0 || ry < 0 ||                    //
           rx >= (I)new_size || ry >= (I)new_size //
    ) {
      if (new_size == 0) {
        new_size = 128;
        rx = new_size / 2;
        ry = new_size / 2;
      } else {
        new_size = new_size * 2;
        rx += new_size / 4;
        ry += new_size / 4;
      }
    }

    if (new_size != this->size) {
      // clang-format really hates this line
      I(*n)
      [new_size][new_size] = alloc(NULL, sizeof(I[new_size][new_size]));

      if (this->size != 0) {
        I(*t)[this->size][this->size] = this->data;
        Z off = (new_size - this->size) / 2;
        for (Z y = 0; y < this->size; y++) {
          memcpy(                //
            &(*n)[off + y][off], //
            &(*t)[y][0],         //
            sizeof((*t)[y]));    //
        }
      }

      this->data = alloc(this->data, 0);
      this->data = n;
      this->size = new_size;
    }
  }

  // index the grid
  I(*g)[this->size][this->size] = this->data;
  return &(*g)[ry][rx];
}
