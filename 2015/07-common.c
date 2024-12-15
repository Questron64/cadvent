typedef struct {
  enum { NUMBER, NODE } type;
  U16 value;
} Value;

typedef struct {
  enum { VALUE, NOT, AND, OR, LSHIFT, RSHIFT } type;
  Value params[2];
} Node;

Node net[UINT16_MAX];

U16 str_to_idx(CC *str) {
  if (strlen(str) > 2)
    die("invalid node idx: %s", str);
  return (U16)str[0] | ((U16)str[1] << 8);
}

B str_to_num(CC str[6], U16 *n16) {
  errno = 0;
  C *end;
  UL n = strtoul(str, &end, 10);
  if (end != str && errno == 0) {
    if (n > UINT16_MAX)
      die("range error: %s", str);
    *n16 = (U16)n;
    return true;
  }
  return false;
}

Value str_to_value(CC str[6]) {
  if (str != NULL) {
    U16 x;
    if (str_to_num(str, &x))
      return (Value){NUMBER, x};
    else
      return (Value){NODE, str_to_idx(str)};
  }
  return (Value){NUMBER, 0};
}

V insertNode(CC a[6], I type, CC b[6], CC c[6]) {
  net[str_to_idx(c)] = (Node){type, {str_to_value(a), str_to_value(b)}};
}

V loadNet() {
  C *line = NULL;
  Z lineSize = 0;

  while (fgetline(stdin, &line, &lineSize)) {
    C a[6], b[6], c[6];

    if (sscanf(line, "%5s -> %5s", a, b) == 2)
      insertNode(a, VALUE, 0, b);
    else if (sscanf(line, "NOT %5s -> %5s", a, b) == 2)
      insertNode(a, NOT, 0, b);
    else if (sscanf(line, "%5s AND %5s -> %5s", a, b, c) == 3)
      insertNode(a, AND, b, c);
    else if (sscanf(line, "%5s OR %5s -> %5s", a, b, c) == 3)
      insertNode(a, OR, b, c);
    else if (sscanf(line, "%5s LSHIFT %5s -> %5s", a, b, c) == 3)
      insertNode(a, LSHIFT, b, c);
    else if (sscanf(line, "%5s RSHIFT %5s -> %5s", a, b, c) == 3)
      insertNode(a, RSHIFT, b, c);
    else
      die("input error: %s", line);
  }
}

U16 solve(U16 n) {
  U16 params[2];
  for (Z i = 0; i < 2; i++) {
    switch (net[n].params[i].type) {
    case NUMBER: params[i] = net[n].params[i].value; break;
    case NODE:
      params[i] = solve(net[n].params[i].value);
      net[n].params[i] = (Value){NUMBER, params[i]};
      break;
    default: die("invalid value type: %d", net[n].params[i].type);
    }
  }

  switch (net[n].type) {
  case VALUE: return params[0];
  case NOT: return ~params[0];
  case AND: return params[0] & params[1];
  case OR: return params[0] | params[1];
  case LSHIFT: return params[0] << params[1];
  case RSHIFT: return params[0] >> params[1];
  default: die("invalid node type: %d", net[n].type);
  }
}
