// a hash table with arbitrary keys and values
//
// tables can optionally have string keys or values
// if a table has a string key or value then inters the string

// hashing functions
// note that these cannot return 0
#define FNV32_PRIME 16777619
#define FNV32_OFFSET 2166136261

// hash of a fixed-size object
U32 fnv32(CC *s, Z size) {
  U32 h = FNV32_OFFSET;
  for (Z i = 0; i < size; i++) {
    h *= FNV32_PRIME;
    h ^= s[i];
  }
  if (h == 0)
    h++;
  return h;
}

// hash of a string, _size is ignored
U32 fnv32_string(CC *s, Z _size) {
  U32 h = FNV32_OFFSET;
  for (CC *c = s; *c; c++) {
    h *= FNV32_PRIME;
    h ^= *c;
  }
  if (h == 0)
    h++;
  return h;
}

typedef struct {
  Z _key_size;   // size of key in bytes (if 0, keys are strings)
  Z _value_size; // size of value in bytes (if 0, values are strings)
  Z _size;       // number of entries
  Z _capacity;   // max entries
  V *_entries;   // the actual table

  U32 (*_func)(CC *, Z); // the hash function
} Table;

#define TABLE_ENTRIES(CAP, KEY, VAL)                                           \
  struct {                                                                     \
    U32 count[CAP];                                                            \
    U32 hash[CAP];                                                             \
    C key[CAP][KEY ? KEY : sizeof(C *)];                                       \
    C value[CAP][VAL ? VAL : sizeof(C *)];                                     \
  }

// forward declarations (these functions call each other)
B table_get(Table *this, V *key, Z *out_idx, CV **out_key, V **out_value);
V table_reserve(Table *this, Z capacity);

// getters (never touch _underscore members outside of this file)
Z table_key_size(Table *this) { return this->_key_size; }
Z table_value_size(Table *this) { return this->_value_size; }
Z table_size(Table *this) { return this->_size; }
Z table_capacity(Table *this) { return this->_capacity; }
U32 (*table_func(Table *this))(CC *, Z) { return this->_func; }

// initialize a new table and call table_reserve
Table table_init(Z key_size, Z value_size, Z reserve) {
  Table this = {
    ._key_size = key_size,
    ._value_size = value_size,
    ._func = key_size ? fnv32 : fnv32_string,
  };
  table_reserve(&this, reserve);
  return this;
}

// insert a key/value into a table
// return true if insertion was successful
//
// set out_value to the value, either the value just inserted, or
// the existing value
B table_insert(Table *this, V *key, Z key_len, V *value, Z *out_idx, V **out_value) {
  if (table_get(this, key, 0, 0, out_value))
    return false;

  if(this->_key_size == 0 && key_len == 0)
    key_len = strlen(key);

  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  if (this->_size == this->_capacity)
    die("table full");

  U32 hash = this->_func(key, this->_key_size);
  for (U32 idx = hash % this->_capacity, tries = 0; //
       tries < this->_capacity;                     //
       tries++, idx = (idx + 1) % this->_capacity)  //
  {
    if (entries->hash[idx] == 0) {
      entries->count[hash % this->_capacity]++;
      entries->hash[idx] = hash;

      if (this->_key_size == 0)
        *(C **)entries->key[idx] = strndup(key, key_len);
      else
        memcpy(entries->key[idx], key, this->_key_size);

      if (this->_value_size == 0)
        *(C **)entries->value[idx] = strdup(value);
      else
        memcpy(entries->value[idx], value, this->_value_size);

      this->_size++;

      if (out_idx)
        *out_idx = idx;
      if (out_value)
        *out_value = entries->value[idx];
      return true;
    }
  }

  // should be unreachable
  die("unreachable");
}

// reserve entries for a table
//
// this can be a very expensive operation
// you should reserve more than you need rather than resize
// larger tables reduce load factor at the cost of memory
//
// it is an error to shrink a table to a capacity less than its size
// except a size of 0
//
// reserve to a capacity of 0 will free all memory used by the table
//
// reserve to a capacity of _capacity will defrag the table
// table fragmentation occurs on tables with high load factor and frequent
// deletions, causing entries to be further from their intended position
V table_reserve(Table *this, Z capacity) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  // free table
  if (capacity == 0) {
    // free string keys/values
    if (this->_key_size == 0 || this->_value_size == 0) {
      for (Z i = 0; i < this->_capacity; i++) {
        if (entries->hash[i] != 0) {
          if (this->_key_size == 0)
            *(V **)entries->key[i] = alloc(*(V **)entries->key[i], 0);
          if (this->_value_size == 0)
            *(V **)entries->value[i] = alloc(*(V **)entries->value[i], 0);
        }
      }
    }
    this->_entries = alloc(this->_entries, 0);
    this->_capacity = 0;
    return;
  }

  if (capacity < this->_size)
    die("cannot shrink hash to smaller than size");

  // create a new table
  Table new = *this;

  typedef TABLE_ENTRIES(capacity, this->_key_size, this->_value_size)
    NewTableEntries;
  new._entries = alloc(0, sizeof(NewTableEntries));
  new._capacity = capacity;

  // iterate the old table and insert into new
  for (Z i = 0; i < this->_capacity; i++)
    if (entries->hash[i])
      table_insert(&new, entries->key[i], 0, entries->value[i], 0, 0);

  this->_entries = alloc(this->_entries, 0);
  *this = new;
}

B table_get(Table *this, V *key, Z *out_idx, CV **out_key, V **out_value) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  U32 hash = this->_func(key, this->_key_size);
  U32 hash_round = hash % this->_capacity;
  if (entries->count[hash_round] == 0)
    return false;

  // start at hash_round and look forward for the real match
  //
  // in a table with a low load factor this should be the first
  // entry or just a few entries away from the start
  for (U32 idx = hash % this->_capacity, tries = 0, hits = 0;         //
       hits <= entries->count[hash_round] && tries < this->_capacity; //
       tries++, idx = (idx + 1) % this->_capacity)                    //
  {
    if (hash_round == entries->hash[idx] % this->_capacity)
      hits++;
    if (entries->hash[idx] == hash) {
      if (out_idx)
        *out_idx = (Z)idx;
      if (out_key)
        *out_key = entries->key[idx];
      if (out_value)
        *out_value = entries->value[idx];
      return true;
    }
  }

  // not found
  return false;
}

B table_get_by_idx(Table *this, Z idx, CV **out_key, V **out_value) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  if (idx >= this->_capacity || entries->hash[idx] == 0)
    return false;

  if (out_key)
    *out_key = entries->key[idx];
  if (out_value)
    *out_value = entries->value[idx];
  return true;
}

// delete an entry by index, free interred strings
V table_delete_by_idx(Table *this, Z idx) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  if (entries->hash[idx] == 0 || idx >= this->_capacity)
    die("invalid idx");

  if (this->_key_size == 0)
    *(V **)entries->key[idx] = alloc(*(V **)entries->key[idx], 0);
  if (this->_value_size == 0)
    *(V **)entries->value[idx] = alloc(*(V **)entries->value[idx], 0);
  entries->count[entries->hash[idx] % this->_capacity]--;
  entries->hash[idx] = 0;
}

// get an entry by key, delete it, do not free interred strings
// this should probably not be used if you use string key or value
B table_delete_by_key(Table *this, V *key) {
  Z idx;
  if (table_get(this, key, &idx, NULL, NULL)) {
    table_delete_by_idx(this, idx);
    return true;
  }
  return false;
}

// get an index iterator to the first entry in the table
// this can be used to iterate over all entries
Z table_start(Table *this) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  for (Z idx = 0; idx < this->_capacity; idx++)
    if (entries->hash[idx] != 0)
      return idx;
  return this->_capacity;
}

// given an index, find the next entry in the table
Z table_next(Table *this, Z idx) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  idx++;
  for (; idx < this->_capacity; idx++)
    if (entries->hash[idx] != 0)
      return idx;
  return this->_capacity;
}

Z table_end(Table *this) { return this->_capacity; }
