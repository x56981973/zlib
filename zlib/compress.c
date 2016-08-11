#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "zlib.h"
#include "encrypt.h"

#define CHUNK 16384

void BinToHex(uLongf bin, unsigned char *hex)
{
    int buf[16] = {0};
    int i = 15;
    while(bin != 0)
    {
        buf[i] = (bin % 2) ? 1 : 0;
        bin /= 2;
        i--;
    }
    for(i = 0; i < 4; i++){
        int c = buf[i*4]*8 + buf[i*4+1]*4 + buf[i*4+2]*2 + buf[i*4+3]*1;
        if(c > 9)
        {
            hex[i] = c + 55;
        }
        else
        {
            hex[i] = c + 48;
        }
    }
}

void HexToBin(unsigned char *hex, uLongf *bin)
{
    int buf[4];
    int i;
    for(i = 0; i < 4; i++){
        int c = hex[i];
        if(c > 64){
            buf[i] = c - 55;
        } else {
            buf[i] = c - 48;
        }
    }

    *bin = buf[0]*16*16*16 + buf[1]*16*16 + buf[2]*16 + buf[3];
}

int CompressAndEncrypt(FILE *src, FILE *dst, unsigned char *key)
{
    unsigned char block_in[CHUNK];
    unsigned char *block_out, *buf, *cipher_text;
    unsigned char flag[4], cipher_flag[AES_BLOCK_SIZE];

    uLongf byte_read, byte_write;
    int block_num;
    int ret;

    while(1){
        //Compress Data
        byte_read = fread(block_in, 1, CHUNK, src);
        byte_write = compressBound(byte_read);
        buf = (unsigned char *)malloc(byte_write);
        ret = compress((Bytef *)buf, &byte_write, (Bytef *)block_in, byte_read);
        if(ret != Z_OK)
        {
            printf("Compress Error code : %d \n",ret);
            return -1;
        }

        //Encrypt Flag
        BinToHex(byte_write, flag);
        ret = encrypt_cbc(flag, cipher_flag, 4, key);
        if(ret < 0){
            printf("Encrypt Error code : %d \n",ret);
            return -1;
        }

        //Encrypt Data
        block_num = byte_write / AES_BLOCK_SIZE + 1;
        cipher_text = (unsigned char *)malloc(AES_BLOCK_SIZE * block_num);
        ret = encrypt_cbc(buf, cipher_text, byte_write, key);
        if(ret < 0){
            printf("Encrypt Error code : %d \n",ret);
            return -1;
        }

        //Write File
        int block_out_length = AES_BLOCK_SIZE * block_num + AES_BLOCK_SIZE;
        block_out = (unsigned char *)malloc(block_out_length);
        memcpy(block_out, cipher_flag, AES_BLOCK_SIZE);
        memcpy(block_out + AES_BLOCK_SIZE, cipher_text, AES_BLOCK_SIZE * block_num);
        fwrite(block_out, 1, block_out_length, dst);

        //clean
        free(buf);
        free(cipher_text);
        free(block_out);

        if(byte_read < CHUNK){
            break;
        }
    }

    return 1;
}

int UncompressAndDecrypt(FILE *src, FILE *dst, unsigned char *key)
{
    unsigned char block_out[CHUNK];
    unsigned char *buf, *cipher_text;
    unsigned char flag[4],cipher_flag[AES_BLOCK_SIZE];

    uLongf byte_read,byte_write;
    int ret;
    int block_num;

    while(1){
        if(fread(cipher_flag, 1, AES_BLOCK_SIZE, src) != AES_BLOCK_SIZE){
            break;
        }

        //Decrypt Flag
        ret = decrypt_cbc(cipher_flag, flag, 4, key);
        if(ret < 0){
            printf("Decrypt Error code : %d \n",ret);
            return -1;
        }
        HexToBin(flag, &byte_read);

        //Decrypt Data
        block_num = byte_read / AES_BLOCK_SIZE + 1;
        cipher_text = (unsigned char *)malloc(AES_BLOCK_SIZE * block_num);
        fread(cipher_text, 1, AES_BLOCK_SIZE * block_num, src);

        buf = (unsigned char *)malloc(byte_read);
        ret = decrypt_cbc(cipher_text, buf, byte_read, key);
        if(ret < 0){
            printf("Decrypt Error code : %d \n",ret);
            return -1;
        }

        //Uncompress Data
        byte_write = CHUNK;
        ret = uncompress((Bytef *)block_out, &byte_write, (Bytef *)buf, byte_read);
        if(ret != Z_OK){
            printf("Uncompress Error code : %d \n",ret);
            return -1;
        }
        fwrite(block_out, 1, byte_write, dst);

        //clean
        free(buf);
        free(cipher_text);
    }

    return 1;
}
