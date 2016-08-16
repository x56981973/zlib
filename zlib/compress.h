#include <stdio.h>
#include "zlib.h"

#define CHUNK 16384

int CompressAndEncrypt(int src, int dst, unsigned char *key);

int UncompressAndDecrypt(int src, int dst, unsigned char *key);
