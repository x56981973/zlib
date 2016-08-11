#include <stdio.h>
#include "zlib.h"

#define CHUNK 16384

int CompressAndEncrypt(FILE *src, FILE *dst, unsigned char *key);

int UncompressAndDecrypt(FILE *src, FILE *dst, unsigned char *key);
