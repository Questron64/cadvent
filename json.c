typedef enum {
  JSON_INVALID,
  JSON_STRING,
  JSON_NUMBER,
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_BOOLEAN,
  JSON_NULL,
} JSONType;

typedef struct {
  C *buf;    // buffer being parsed
  Z buf_len; // length of buffer
  Z idx;     // index into buffer

  V *user;
  V (*on_number)(C *num, Z num_len, V *user);
} JSONParserState;

B json_match(CC *str, JSONParserState *p);
B json_match_any(CC *c, JSONParserState *p);
B json_json(JSONParserState *p);
B json_value(JSONParserState *p);
B json_object(JSONParserState *p);
B json_members(JSONParserState *p);
B json_member(JSONParserState *p);
B json_array(JSONParserState *p);
B json_elements(JSONParserState *p);
B json_element(JSONParserState *p);
B json_string(JSONParserState *p);
B json_characters(JSONParserState *p);
B json_character(JSONParserState *p);
B json_escape(JSONParserState *p);
B json_hex(JSONParserState *p);
B json_number(JSONParserState *p);
B json_integer(JSONParserState *p);
B json_digits(JSONParserState *p);
B json_digit(JSONParserState *p);
B json_onenine(JSONParserState *p);
B json_fraction(JSONParserState *p);
B json_exponent(JSONParserState *p);
B json_sign(JSONParserState *p);
B json_ws(JSONParserState *p);

B json_match(CC *str, JSONParserState *p) {
  Z len = strlen(str);
  if (p->idx + len < p->buf_len && !strncmp(&p->buf[p->idx], str, len)) {
    p->idx += len;
    return true;
  }
  return false;
}

B json_match_any(CC *c, JSONParserState *p) {
  if (p->idx < p->buf_len && strchr(c, p->buf[p->idx])) {
    (p->idx)++;
    return true;
  }
  return false;
}

B json_json(JSONParserState *p) { return json_element(p); }

B json_value(JSONParserState *p) {
  return                      //
    json_object(p) ||         //
    json_array(p) ||          //
    json_string(p) ||         //
    json_number(p) ||         //
    json_match("true", p) ||  //
    json_match("false", p) || //
    json_match("null", p);
}

B json_object(JSONParserState *p) {
  Z back = p->idx;
  if (                    //
    json_match("{", p) && //
    json_ws(p) &&         //
    json_match("}", p)    //
  ) {
    return true;
  }

  p->idx = back;
  if (                    //
    json_match("{", p) && //
    json_members(p) &&    //
    json_match("}", p)    //
  ) {
    return true;
  }

  p->idx = back;
  return false;
}

B json_members(JSONParserState *p) {
  if (!json_member(p))
    return false;
  Z back = p->idx;
  while (json_match(",", p) && json_member(p))
    back = p->idx;
  p->idx = back;
  return true;
}

B json_member(JSONParserState *p) {
  Z back = p->idx;
  if (                    //
    json_ws(p) &&         //
    json_string(p) &&     //
    json_ws(p) &&         //
    json_match(":", p) && //
    json_element(p)       //
  ) {
    return true;
  }
  p->idx = back;
  return false;
}

B json_array(JSONParserState *p) {
  Z back = p->idx;
  if (                    //
    json_match("[", p) && //
    json_ws(p) &&         //
    json_match("]", p)    //
  ) {
    return true;
  }

  p->idx = back;
  if (                    //
    json_match("[", p) && //
    json_elements(p) &&   //
    json_match("]", p)    //
  ) {
    return true;
  }

  p->idx = back;
  return false;
}

B json_elements(JSONParserState *p) {
  if (!json_element(p))
    return false;
  Z back = p->idx;
  while (json_match(",", p) && json_element(p))
    back = p->idx;
  p->idx = back;
  return true;
}

B json_element(JSONParserState *p) {
  Z back = p->idx;
  if (               //
    json_ws(p) &&    //
    json_value(p) && //
    json_ws(p)       //
  ) {
    return true;
  }
  p->idx = back;
  return false;
}

B json_string(JSONParserState *p) {
  Z back = p->idx;
  if (                     //
    json_match("\"", p) && //
    json_characters(p) &&  //
    json_match("\"", p)    //
  ) {
    return true;
  }
  p->idx = back;
  return false;
}

B json_characters(JSONParserState *p) {
  while (json_character(p))
    ;
  return true;
}

B json_character(JSONParserState *p) {
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

B json_escape(JSONParserState *p) {
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

B json_hex(JSONParserState *p) {
  return json_match_any("0123456789abcdefABCDEF", p);
}

B json_number(JSONParserState *p) {
  Z back = p->idx;
  if (                  //
    json_integer(p) &&  //
    json_fraction(p) && //
    json_exponent(p)    //
  ) {
    if (p->on_number)
      p->on_number(&p->buf[back], p->idx - back, p->user);
    return true;
  }
  p->idx = back;
  return false;
}

B json_integer(JSONParserState *p) {
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

B json_digits(JSONParserState *p) {
  Z back = p->idx;
  while (json_digit(p))
    ;
  return p->idx > back;
}

B json_digit(JSONParserState *p) { return json_match_any("0123456789", p); }
B json_onenine(JSONParserState *p) { return json_match_any("123456789", p); }

B json_fraction(JSONParserState *p) {
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

B json_exponent(JSONParserState *p) {
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

B json_sign(JSONParserState *p) { return json_match_any("+-", p); }

B json_ws(JSONParserState *p) {
  while (json_match_any("\x20\x0a\x0d\x09", p))
    ;
  return true;
}
