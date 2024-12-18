typedef enum {
  JSON_INVALID,
  JSON_STRING,
  JSON_NUMBER,
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_BOOLEAN,
  JSON_NULL,
} JSONType;

typedef struct JSONValue JSONValue;
typedef struct JSONArray JSONArray;

struct JSONArray {
  Z size;
  JSONValue *values;
};

struct JSONValue {
  JSONType type;
  union {
    C *string;
    D number;
    Table object;
    JSONArray array;
    B boolean;
  };
};

typedef struct {
  CC *buf;   // buffer being parsed
  Z buf_len; // length of buffer
  Z idx;     // index into buffer
} JSON;

JSON json_parser_init(CC *buf, Z buf_len);

V json_free(JSONValue *v);
V json_decode_string(C *str);

B json_parse(JSON *p, JSONValue *val);
B json_value(JSON *p, JSONValue *val);
B json_object(JSON *p, JSONValue *val);
B json_member(JSON *p, B comma, JSONValue *val);
B json_array(JSON *p, JSONValue *val);
B json_element(JSON *p, JSONValue *val);
B json_string(JSON *p, JSONValue *val);
B json_characters(JSON *p);
B json_character(JSON *p);
B json_escape(JSON *p);
B json_hex(JSON *p);
B json_number(JSON *p, JSONValue *val);
B json_boolean(JSON *p, JSONValue *val);
B json_null(JSON *p, JSONValue *val);
B json_integer(JSON *p);
B json_digits(JSON *p);
B json_digit(JSON *p);
B json_onenine(JSON *p);
B json_fraction(JSON *p);
B json_exponent(JSON *p);
B json_sign(JSON *p);
B json_ws(JSON *p);
B json_match(CC *str, JSON *p);
B json_match_any(CC *c, JSON *p);

JSON json_parser_init(CC *buf, Z buf_len) {
  return (JSON){
    .buf = buf,
    .buf_len = buf_len,
  };
}

// recursively free all objects under v, but not v
V json_free(JSONValue *v) {
  switch (v->type) {
  case JSON_STRING: v->string = alloc(v->string, 0); break;
  case JSON_OBJECT:
    for (Z i = table_start(&v->object); //
         i != table_end(&v->object);    //
         i = table_next(&v->object, i)  //
    ) {
      JSONValue *v2;
      if (!table_get_by_idx(&v->object, i, 0, (V **)&v2))
        die("table index failure");
      json_free(v2);
      table_delete_by_idx(&v->object, i);
    }
    table_reserve(&v->object, 0);
    break;
  case JSON_ARRAY:
    for (Z i = 0; i < v->array.size; i++)
      json_free(&v->array.values[i]);
    v->array.values = alloc(v->array.values, 0);
    break;
  default:
  case JSON_INVALID:
  case JSON_NUMBER:
  case JSON_BOOLEAN:
  case JSON_NULL: break;
  }
  *v = (JSONValue){JSON_INVALID};
}

V json_print(JSONValue *v, I indent) {
  CC *spaces =                                           //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  " //
    "                                                  ";

  switch (v->type) {
  case JSON_INVALID: die("cannot print invalid json value");
  case JSON_STRING: printf("\"%s\"\n", v->string); break;
  case JSON_NUMBER: printf("%f\n", v->number); break;
  case JSON_BOOLEAN: printf("%s\n", v->boolean ? "true" : "false"); break;
  case JSON_NULL: printf("null\n"); break;
  case JSON_OBJECT:
    printf("{\n");
    for (Z i = table_start(&v->object); //
         i != table_end(&v->object);    //
         i = table_next(&v->object, i)  //
    ) {
      C **key;
      JSONValue *v2;
      table_get_by_idx(&v->object, i, (CV **)&key, (V **)&v2);
      printf("%.*s\"%s\" = ", indent + 2, spaces, *key);
      json_print(v2, indent + 4);
    }
    printf("%.*s}\n", indent, spaces);
    break;
  case JSON_ARRAY:
    printf("[\n");
    for (Z i = 0; i < v->array.size; i++) {
      printf("%.*s", indent + 2, spaces);
      json_print(&v->array.values[i], indent + 4);
    }
    printf("%.*s]\n", indent, spaces);
    break;
  }
}

V json_decode_string(C *str) {}

B json_parse(JSON *p, JSONValue *val) { //
  return json_element(p, val);
}

B json_value(JSON *p, JSONValue *val) {
  return                    //
    json_object(p, val) ||  //
    json_array(p, val) ||   //
    json_string(p, val) ||  //
    json_number(p, val) ||  //
    json_boolean(p, val) || //
    json_null(p, val);      //
}

B json_object(JSON *p, JSONValue *val) {
  Z back = p->idx;

  JSONValue object = {0};
  if (val) {
    object = (JSONValue){
      JSON_OBJECT,                                      //
      .object = table_init(0, sizeof(JSONValue), 1000), //
    };
  }

  if (!json_match("{", p))
    goto fail;

  if(!json_member(p, false, val ? &object : NULL))
    goto fail;
  while(json_member(p, true, val ? &object : NULL))
    ;

  json_ws(p);
  if (!json_match("}", p))
    goto fail;

  if (val)
    *val = object;
  return true;

fail:
  if(val)
    json_free(&object);
  p->idx = back;
  return false;
}

B json_member(JSON *p, B comma, JSONValue *object) {
  Z back = p->idx;

  json_ws(p);
  if(comma && !json_match(",", p))
    goto fail;
  json_ws(p);

  Z key_start = p->idx + 1;
  if (!json_string(p, NULL))
    goto fail;
  Z key_end = p->idx - 1;

  json_ws(p);
  if (!json_match(":", p))
    goto fail;

  JSONValue val = {0};
  if (!json_element(p, object ? &val : NULL))
    goto fail;

  if (object)
    table_insert(
      &object->object, (V*)&p->buf[key_start], key_end - key_start, &val, NULL,
      NULL);

  return true;

fail:
  p->idx = back;
  return false;
}

B json_array(JSON *p, JSONValue *val) {
  Z back = p->idx;
  if (                    //
    json_match("[", p) && //
    json_ws(p) &&         //
    json_match("]", p)    //
  ) {
    if (val)
      *val = (JSONValue){JSON_ARRAY, .array = {0}};
    return true;
  }

  p->idx = back;
  JSONValue a = {JSON_ARRAY};

  if (!json_match("[", p))
    goto fail;

  for (I i = 0;; i++) {
    if (i > 0 && !json_match(",", p))
      break;

    JSONValue item = {0};
    if (!json_element(p, val ? &item : NULL))
      goto fail;

    if (val) {
      a.array.size++;
      a.array.values = alloc(a.array.values, sizeof(JSONValue) * a.array.size);
      a.array.values[a.array.size - 1] = item;
    }
  }

  if (!json_match("]", p))
    goto fail;

  if (val)
    *val = a;
  return true;

fail:
  if (val)
    json_free(&a);
  p->idx = back;
  return false;
}

B json_element(JSON *p, JSONValue *val) {
  Z back = p->idx;
  if (                    //
    json_ws(p) &&         //
    json_value(p, val) && //
    json_ws(p)            //
  ) {
    return true;
  }

  p->idx = back;
  return false;
}

B json_string(JSON *p, JSONValue *val) {
  Z back = p->idx;
  if (                     //
    json_match("\"", p) && //
    json_characters(p) &&  //
    json_match("\"", p)    //
  ) {
    if (val) {
      C *str = strndup(&p->buf[back + 1], p->idx - back - 2);
      if (str == NULL)
        die("memory error");
      json_decode_string(str);
      *val = (JSONValue){JSON_STRING, .string = str};
    }

    return true;
  }
  p->idx = back;
  return false;
}

B json_characters(JSON *p) {
  while (json_character(p))
    ;
  return true;
}

B json_character(JSON *p) {
  if (
    p->idx < p->buf_len &&    //
    p->buf[p->idx] >= 0x20 && //
    p->buf[p->idx] != '"' &&  //
    p->buf[p->idx] != '\\'    //
  ) {
    (p->idx)++;
    return true;
  }
  Z back = p->idx;
  if (                     //
    json_match("\\", p) && ///
    json_escape(p)         //
  ) {
    return true;
  }
  p->idx = back;
  return false;
}

B json_escape(JSON *p) {
  Z back = p->idx;
  if (                                 //
    json_match_any("\"\\/bfnrt", p) || //
    (json_match("u", p) &&             //
     json_hex(p) &&                    //
     json_hex(p) &&                    //
     json_hex(p) &&                    //
     json_hex(p))                      //
  ) {
    return true;
  }
  p->idx = back;
  return false;
}

B json_hex(JSON *p) { return json_match_any("0123456789abcdefABCDEF", p); }

B json_number(JSON *p, JSONValue *val) {
  Z back = p->idx;
  if (                  //
    json_integer(p) &&  //
    json_fraction(p) && //
    json_exponent(p)    //
  ) {
    if (val) {
      C *end = NULL;
      errno = 0;
      *val = (JSONValue){
        JSON_NUMBER,                          //
        .number = strtod(&p->buf[back], &end) //
      };
      if (errno != 0 || end != &p->buf[p->idx])
        die("cannot parse number");
    }
    return true;
  }
  p->idx = back;
  return false;
}

B json_boolean(JSON *p, JSONValue *val) {
  if (json_match("true", p)) {
    if (val)
      *val = (JSONValue){JSON_BOOLEAN, .boolean = true};
    return true;
  } else if (json_match("false", p)) {
    if (val)
      *val = (JSONValue){JSON_BOOLEAN, .boolean = false};
    return true;
  }
  return false;
}

B json_null(JSON *p, JSONValue *val) {
  if (json_match("null", p)) {
    if (val)
      *val = (JSONValue){JSON_NULL};
    return true;
  }
  return false;
}

B json_integer(JSON *p) {
  Z back = p->idx;
  if (                              //
    (json_match("-", p) || true) && //
    json_digits(p)                  //
  ) {
    return true;
  }
  p->idx = back;
  return false;
}

B json_digits(JSON *p) {
  Z back = p->idx;
  while (json_digit(p))
    ;
  return p->idx > back;
}

B json_digit(JSON *p) { return json_match_any("0123456789", p); }
B json_onenine(JSON *p) { return json_match_any("123456789", p); }

B json_fraction(JSON *p) {
  Z back = p->idx;
  if (                    //
    json_match(".", p) && //
    json_digits(p)        //
  ) {
    return true;
  }
  p->idx = back;
  return true;
}

B json_exponent(JSON *p) {
  Z back = p->idx;
  if (                         //
    json_match_any("eE", p) && //
    json_sign(p) &&            //
    json_digits(p)             //
  ) {
    return true;
  }
  p->idx = back;
  return true;
}

B json_sign(JSON *p) { return json_match_any("+-", p); }

B json_ws(JSON *p) {
  while (json_match_any("\x20\x0a\x0d\x09", p))
    ;
  return true;
}

B json_match(CC *str, JSON *p) {
  Z len = strlen(str);
  if (p->idx + len < p->buf_len && !strncmp(&p->buf[p->idx], str, len)) {
    p->idx += len;
    return true;
  }
  return false;
}

B json_match_any(CC *c, JSON *p) {
  if (p->idx < p->buf_len && strchr(c, p->buf[p->idx])) {
    (p->idx)++;
    return true;
  }
  return false;
}
