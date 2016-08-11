#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include "zlib.h"
#include "compression.h"

#define SIZE 1024
#define AES_SIZE 16
#define CHUNK 1024

int main(int argc, char *argv[])
{
    char *readFile = "/home/guest/senseThink/test2/test2/com";
    char *writeFileEncrypt = "/home/guest/senseThink/test2/test2/ee.txt";
    char *writeFileDecrypt = "/home/guest/senseThink/test2/test2/result";

    FILE *fRead = NULL;
    FILE *fWriteEncrypt = NULL;
    FILE *fWriteDecrypt = NULL;

    unsigned char bufferFile[CHUNK];

    unsigned char bufferCompressed[CHUNK];


    void condense(char* filePath);
    unsigned char bufferKey[SIZE];
    unsigned char bufferOut[CHUNK];
    unsigned char bufferOut2[CHUNK];

    void decompression(char* filePath,char*endPath);
    memset(bufferFile,0,CHUNK);
    memset(bufferKey,0,SIZE);
    memset(bufferOut,0,CHUNK);
    memset(bufferOut2,0,CHUNK);
    int ret;
    int byteRead = 0;
    //int size = 0;

    void getKey(char* keyPath, char* bufferKey);
    void encrypt( unsigned char* key, unsigned char* temp, unsigned char* out);
    void decrypt( unsigned char* key, unsigned char* temp, unsigned char* out);
    printf("%d\n%s\n%s\n%s\n",argc,argv[3],argv[5],argv[7]);

    if(argc == 7)
    {
        if((strcmp(argv[1],"-k")==0)&&(strcmp(argv[3],"-i")==0)&&(strcmp(argv[5],"-o")==0))
        {
            //condense(argv[4]);
            getKey(argv[2], bufferKey);
            //readFile = "/home/guest/senseThink/test2/test2/com";
            readFile = argv[4];
            writeFileEncrypt = argv[6];
            if((fRead = fopen(readFile,"rb")) == NULL)
            {
                fprintf(stderr, "Cannot open read file1.\n");
                return ;
            }
            if((fWriteEncrypt = fopen(writeFileEncrypt,"wb")) == NULL)
            {
                fprintf(stderr, "Cannot open encrypt file1.\n");
                return ;
            }

            //fseek(fRead, 0L, SEEK_END);
            //size = ftell(fRead);
            //printf("1111%d",size);
            //fputc(size,fWriteEncrypt);
            //fseek(fRead, 0L, 0);


            while( (byteRead = fread(bufferFile, 1, CHUNK, fRead)) > 0)
             {
                if(byteRead < 1024)
                {
                    for(int i = byteRead;i<1024;i++)
                    {
                        bufferFile[i] = ' ';
                    }
                }
                uLongf sizeCompressed = compressBound(byteRead);
                char bufferCompressed[CHUNK];
                ret = compress( (Bytef*) bufferCompressed, &sizeCompressed, (Bytef*)bufferFile, SIZE);
                if (ret != Z_OK)
                    zerr(ret);

                //uLongf sizeDecompress = SIZE;
                //ret = uncompress( (Bytef*)bufferOut, &sizeDecompress, (Bytef*)bufferCompressed, SIZE);
                //char * write =

                fwrite(&sizeCompressed,1,4,fWriteEncrypt);
                fwrite(bufferCompressed, 1, sizeCompressed, fWriteEncrypt);

                //printf("inwhile:%s",bufferFile);
                //encrypt(bufferKey, bufferCompressed, bufferOut);
                //fwrite(bufferOut, AES_SIZE, 1, fWriteEncrypt);
             }

            fclose(fRead);
            fclose(fWriteEncrypt);
        }
    }else if(argc == 8)
    {
        int count = 1024;
        if((strcmp(argv[2],"-k")==0)&&(strcmp(argv[4],"-i")==0)&&(strcmp(argv[6],"-o")==0)&&(strcmp(argv[1],"-d")==0))
        {
            getKey(argv[3],bufferKey);
            writeFileEncrypt = argv[5];
            writeFileDecrypt = "/home/guest/senseThink/test2/test2/result";
            //printf("11");
            if((fWriteEncrypt = fopen(writeFileEncrypt,"rb")) == NULL)
            {
                fprintf(stderr, "Cannot open read file2.\n");
                return ;
            }
            if((fWriteDecrypt = fopen(writeFileDecrypt,"wb")) == NULL)
            {
                fprintf(stderr, "Cannot open encrypt file2.\n");
                return ;
            }

            //size = fgetc(fWriteEncrypt);
            fread(&count, 1, 4, fWriteEncrypt);
            while( (byteRead =  fread(bufferFile, 1, count, fWriteEncrypt)) > 0)
             {
//ftell
                //decrypt(bufferKey,bufferFile, bufferOut2);


                uLongf sizeDecompress = 1024;
                ret = uncompress( (Bytef*)bufferOut, &sizeDecompress, (Bytef*)bufferFile, SIZE);
                if (ret != Z_OK)
                    zerr(ret);

                if(sizeDecompress < 1024)//xxxx
                {
                    for(int i = 0;i < sizeDecompress;i++)
                    {
                        bufferOut2[i] = bufferOut[i];
                    }
                    fwrite(bufferOut2, 1, sizeDecompress, fWriteDecrypt);
                    break;

                }

                fwrite(bufferOut, 1, sizeDecompress, fWriteDecrypt);

                //fwrite(bufferOut2, 1, AES_SIZE, fWriteDecrypt);
                fread(&count, 1, 4, fWriteEncrypt);
                //printf("111\n%s\n",bufferOut2);
             }
            //printf("222\n%s\n",bufferOut2);
            fclose(fWriteDecrypt);
            fclose(fWriteEncrypt);
            //decompression(writeFileDecrypt,argv[7]);
        }
    }
    printf("boooooooooooooom");
    return 0;
}

void getKey(char* keyPath, char* bufferKey)
{
    FILE *fpKey = NULL;

    //printf("%s",keyPath);
    fpKey = fopen(keyPath,"rb");
    if(fpKey != NULL)
    {
        if(fgets(bufferKey,SIZE,fpKey) != NULL)
        {
            printf("bufferKey:%s\n",bufferKey);
        }else
        {
            return ;
        }
    }else
    {
        return ;
    }
    fclose(fpKey);
    fpKey = NULL;
}







void condense(char* filePath)
{
    int ret;
    FILE* file = NULL;
    FILE* fpFile = NULL;

    if((file = fopen(filePath,"r")) == NULL)
    {
        fprintf(stderr, "Cannot open output file.\n");
        return;
    }

    fpFile = fopen("/home/guest/senseThink/test2/test2/com","wb");
    ret = def(file, fpFile, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        printf("sadddddd\n");
    fclose(fpFile);
    fclose(file);
}

void decompression(char* filePath, char* endPath)
{
    int ret;
    printf("\n%s\n",filePath);
    FILE* file = NULL;
    FILE* fpFile = NULL;
    if((file = fopen(filePath,"rb")) == NULL)
    {
        fprintf(stderr, "Cannot open output file.\n");
        return ;
    }
    fpFile = fopen(endPath,"wb");
    if(fpFile == NULL)
    {
        printf("NULLLLLL");
    }
    ret = inf(file,fpFile);
    if (ret != Z_OK)
        printf("sadddddd");
    fclose(fpFile);
    fclose(file);
}




void encrypt( unsigned char* key, unsigned char* temp, unsigned char* out)
{
    AES_KEY  aes;
    if (AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }
    AES_encrypt(temp,out,&aes);
}

void decrypt( unsigned char* key, unsigned char* temp, unsigned char* out)
{
    AES_KEY  aes;
    if (AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }
    AES_decrypt(temp, out, &aes);
}



