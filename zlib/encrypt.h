#include <stdio.h>
#include "openssl/aes.h"

int encrypt_cbc(unsigned char *in, unsigned char *out, int length, unsigned char *key, unsigned char *ivec);

int decrypt_cbc(unsigned char *in, unsigned char *out, int length, unsigned char *key, unsigned char *ivec);
