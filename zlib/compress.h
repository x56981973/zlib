#include <stdio.h>
#include "zlib.h"

#define CHUNK 16384

int compressAndEncrypt(int src, int dst, unsigned char *key);

int uncompressAndDecrypt(int src, int dst, unsigned char *key);
