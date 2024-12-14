I findHash(CC *key, CC *hashPrefix) {
  Z len = strlen(key);
  Z pfxLen = strlen(hashPrefix);

  C *buf = alloc(NULL, len + 12); // INT_MAX plus nul
  strcpy(buf, key);

  I i = 0;
  for (; i < INT_MAX; i++) {
    sprintf(&buf[len], "%d", i);
    C hash[MD5_DIGEST_STRING_LENGTH];
    MD5StringHex(buf, hash);
    if(!strncmp(hash, hashPrefix, pfxLen))
      break;
  }

  if (i == INT_MAX)
    die("hash not found");

  buf = alloc(buf, 0);
  return i;
}

C *readInput(FILE *f) {
  C *input = NULL;
  if (fgetline(f, &input, NULL) == 0)
    die("input error");
  trim(input);
  return input;
}

