#include <stdio.h>
#include "zlib.h"

#define CHUNK 16384

void BinToHex(uLongf bin, unsigned char *hex);

void HexToBin(unsigned char *hex, uLongf *bin);

int CompressAndEncrypt(FILE *src, FILE *dst, unsigned char *key);

int UncompressAndDecrypt(FILE *src, FILE *dst, unsigned char *key);
