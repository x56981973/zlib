#ifndef COMPRESSION
#define COMPRESSION
int compression(int number, char **path);

int def(FILE *source, FILE *dest, int level);
int inf(FILE *source, FILE *dest);
#endif // COMPRESSION

