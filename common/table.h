#ifndef TABLE_H
#define TABLE_H
#include "common.h"

// hashing functions
U32 fnv32(CC *s, Z size);
U32 fnv32_string(CC *s, Z _size);

// table
typedef struct {
  Z _key_size;   // size of key in bytes (if 0, keys are strings)
  Z _value_size; // size of value in bytes (if 0, values are strings)
  Z _size;       // number of entries
  Z _capacity;   // max entries
  V *_entries;   // the actual table

  U32 (*_func)(CC *, Z); // the hash function
} Table;

static inline Z table_key_size(Table *this) { return this->_key_size; }
static inline Z table_value_size(Table *this) { return this->_value_size; }
static inline Z table_size(Table *this) { return this->_size; }
static inline Z table_capacity(Table *this) { return this->_capacity; }
static inline U32 (*table_func(Table *this))(CC *, Z) { return this->_func; }

Table table_init(Z key_size, Z value_size, Z reserve);
B table_insert(               //
  Table *this,                //
  V *key, Z key_len,          //
  V *value,                   //
  Z *out_idx, V **out_value); //
V table_reserve(Table *this, Z capacity);
B table_get(Table *this, V *key, Z *out_idx, CV **out_key, V **out_value);
B table_get_by_idx(Table *this, Z idx, CV **out_key, V **out_value);
V table_delete_by_idx(Table *this, Z idx);
B table_delete_by_key(Table *this, V *key);
Z table_start(Table *this);
Z table_next(Table *this, Z idx);
Z table_end(Table *this);

#endif