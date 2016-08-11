#include <stdio.h>
#include <string.h>
#include "compress.h"

void printUsage()
{
    printf("Usage:\n");
    printf("    compress   : -c -k <key> -i <data file> -o <out file>\n");
    printf("    uncompress : -u -k <key> -i <data file> -o <out file>\n");
}

int main(int argc, char **argv)
{
    char *in; //source file name
    char *out; //out file name
    unsigned char *key; //AES Key

    if(argc != 8)
    {
        printUsage();
        return 0;
    }
    else if(strcmp(argv[1], "-c") && strcmp(argv[1], "-u"))
    {
        printUsage();
        return 0;
    }
    else if (strcmp(argv[2], "-k"))
    {
        printUsage();
        return 0;
    }
    else if (strcmp(argv[4], "-i"))
    {
        printUsage();
        return 0;
    }
    else if (strcmp(argv[6], "-o"))
    {
        printUsage();
        return 0;
    }
    else
    {
        key = (unsigned char *)argv[3];
        in = argv[5];
        out = argv[7];
    }

    FILE *input;
    input = fopen(in,"rb");

    FILE *output;
    output = fopen(out,"wb");

    int result;
    if(strcmp(argv[1], "-c") == 0)
    {
        result = CompressAndEncrypt(input, output, key);
        if (result < 0)
        {
            printf("Compress File Error.\n");
        }
    }
    else
    {
        result = UncompressAndDecrypt(input, output, key);
        if (result < 0)
        {
            printf("Uncompress File Error.\n");
        }
    }

    fclose(input);
    fclose(output);

    return 0;
}
