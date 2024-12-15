I find_hash(CC *key, CC *hash_prefix) {
  Z len = strlen(key);
  Z pfx_len = strlen(hash_prefix);

  C *buf = alloc(NULL, len + 12); // INT_MAX plus nul
  strcpy(buf, key);

  I i = 0;
  for (; i < INT_MAX; i++) {
    sprintf(&buf[len], "%d", i);
    C hash[MD5_DIGEST_STRING_LENGTH];
    MD5StringHex(buf, hash);
    if (!strncmp(hash, hash_prefix, pfx_len))
      break;
  }

  if (i == INT_MAX)
    die("hash not found");

  buf = alloc(buf, 0);
  return i;
}

C *read_input(FILE *f) {
  C *input = NULL;
  if (fgetline(f, &input, NULL) == 0)
    die("input error");
  trim(input);
  return input;
}
