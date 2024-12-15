typedef I Box[3];

// reads boxes from FILE f
// assumes boxes and num_boxes are uninitialized
Z read_boxes(FILE *f, Box **boxes, Z *num_boxes) {
  *boxes = NULL;
  *num_boxes = 0;

  C *line = NULL;
  Z line_size = 0;

  while (fgetline(f, &line, &line_size)) {
    Box b;
    if (sscanf(line, "%dx%dx%d", &b[0], &b[1], &b[2]) != 3)
      die("input error");

    Box *new = realloc(*boxes, sizeof(Box) * (*num_boxes + 1));
    if (!new)
      die("memory error");
    *boxes = new;
    (*num_boxes)++;

    memcpy(&(*boxes)[*num_boxes - 1], b, sizeof(b));
  }

  free(line);
  return *num_boxes;
}
