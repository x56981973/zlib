#include <stdio.h>
#include "openssl/aes.h"

#define IV "0000000000000000"

int encrypt_cbc(unsigned char *in, unsigned char *out, int length, unsigned char *key);

int decrypt_cbc(unsigned char *in, unsigned char *out, int length, unsigned char *key);
