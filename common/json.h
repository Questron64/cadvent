#ifndef JSON_H
#define JSON_H
#include "common.h"
#include "table.h"

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

JSONValue json_parse_buffer(CC *buf, Z buf_len);
JSONValue json_parse_file(FILE *f);
V json_free(JSONValue *v);
V json_print(JSONValue *v, I indent);

#endif