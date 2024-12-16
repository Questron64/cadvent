// a hash table with arbitrary keys and values
//
// this can be used as a hash table, a set, or a multiset
//
// tables can optionally have string keys or values
// if a table has a string key or value then it duplicates
// the string, removing from the table gives you ownership
// so don't forget to free them

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

// initialize a new table with capacity of 0
Table table_init(Z key_size, Z value_size) {
  return (Table){
    ._key_size = key_size,
    ._value_size = value_size,
    ._func = key_size ? fnv32 : fnv32_string,
  };
}

// insert a key or key/value into a table
//
// value is ignored for sets
Z table_insert(Table *this, V *key, V *value) {
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
    if (entries->hash[idx] != 0)
      continue;

    entries->count[hash % this->_capacity]++;
    entries->hash[idx] = hash;

    if (this->_key_size == 0) {
      C *tmp = strdup(key);
      memcpy(entries->key[idx], &tmp, sizeof(C *));
    } else
      memcpy(entries->key[idx], key, this->_key_size);

    if (this->_value_size == 0) {
      C *tmp = strdup(value);
      memcpy(entries->value[idx], &tmp, sizeof(C *));
    } else
      memcpy(entries->value[idx], value, this->_value_size);

    this->_size++;
    return (Z)idx;
  }

  // should be unreachable
  die("unreachable");
}

// reserve entries for a table
//
// this can be a very expensive operation on tables with entries
// you should reserve more than you need rather than resize
//
// it is an error to shrink a table to a capacity less than its size
//
// reserve to a capacity of 0 will free all memory used by the table
V table_reserve(Table *this, Z capacity) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  // free table
  if (capacity == 0) {
    // free string keys/values
    if (this->_key_size == 0 || this->_value_size == 0) {
      for (Z i = 0; i < this->_capacity; i++) {
        if (entries->hash[i] == 0)
          continue;

        if (this->_key_size == 0) {
          C *tmp;
          memcpy(&tmp, entries->key[i], sizeof(C *));
          alloc(tmp, 0);
        }
        if (this->_value_size == 0) {
          C *tmp;
          memcpy(&tmp, entries->value[i], sizeof(C *));
          alloc(tmp, 0);
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
      table_insert(&new, entries->key[i], entries->value[i]);

  this->_entries = alloc(this->_entries, 0);
  *this = new;
}

B table_get(Table *this, V *key, Z *out_idx, CV **out_key, V **out_value) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  U32 hash = this->_func(key, this->_key_size);
  if (entries->count[hash % this->_capacity] == 0)
    return false;

  for (U32 idx = hash % this->_capacity, tries = 0; //
       tries < this->_capacity;                     //
       tries++, idx = (idx + 1) % this->_capacity)  //
  {
    if (entries->hash[idx] != hash)
      continue;

    if (out_idx)
      *out_idx = (Z)idx;
    if (out_key)
      *out_key = entries->key[idx];
    if (out_value)
      *out_value = entries->value[idx];

    return true;
  }

  // unreachable?
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

// delete an entry by index, do not free interred strings
V table_delete_by_idx(Table *this, Z idx) {
  typedef TABLE_ENTRIES(this->_capacity, this->_key_size, this->_value_size)
    TableEntries;
  TableEntries *entries = this->_entries;

  if (entries->hash[idx] == 0 || idx >= this->_capacity)
    die("invalid idx");

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
