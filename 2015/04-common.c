I findHash(CC *key, CC *hashPrefix) {
  Z len = strlen(key);
  Z pfxLen = strlen(hashPrefix);

  C *buf = alloc(NULL, len + 12); // INT_MAX plus nul
  strcpy(buf, key);

  I i = 0;
  for (; i < INT_MAX; i++) {
    Z newLen = len + sprintf(&buf[len], "%d", i);

    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (const u_int8_t *)buf, newLen);

    C hash[MD5_DIGEST_LENGTH];
    MD5Final((u_int8_t *)hash, &md5);

    CC *hex = "0123456789ABCDEF";
    C hashHex[MD5_DIGEST_STRING_LENGTH] = "";
    for(Z i = 0; i < MD5_DIGEST_LENGTH; i++) {
      hashHex[i * 2] = hex[hash[i] >> 4 & 0xF];
      hashHex[i * 2 + 1] = hex[hash[i] & 0xF];
    }

    if(!strncmp(hashHex, hashPrefix, pfxLen))
      break;
  }

  if (i == INT_MAX)
    die("hash not found");

  buf = alloc(buf, 0);
  return i;
}
