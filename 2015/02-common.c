typedef int Box[3];

// Reads boxes from FILE f. Assumes boxes and numBoxes
// are uninitialized.
size_t readBoxes(FILE *f, Box **boxes, size_t *numBoxes) {
  *boxes = NULL;
  *numBoxes = 0;

  char *line = NULL;
  size_t lineSize = 0;

  while(fgetline(f, &line, &lineSize)) {
    Box b;
    if (sscanf(line, "%dx%dx%d", &b[0], &b[1], &b[2]) != 3)
      die("input error");

    Box *new = realloc(*boxes, sizeof(Box) * (*numBoxes + 1));
    if(!new)
      die("memory error");
    *boxes = new;
    (*numBoxes)++;

    memcpy(&(*boxes)[*numBoxes - 1], b, sizeof(b));
  }

  free (line);
  return *numBoxes;
}
