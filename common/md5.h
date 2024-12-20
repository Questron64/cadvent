#ifndef MD5_H
#define MD5_H

#define MD5_DIGEST_LENGTH 16
#define MD5_DIGEST_STRING_LENGTH (MD5_DIGEST_LENGTH * 2 + 1)

void MD5StringHex(const char *string, char output[MD5_DIGEST_STRING_LENGTH]);

#endif