#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "zlib.h"
#include "compress.h"
#include "encrypt.h"

int getBlockNum(int size)
{
    if( size % AES_BLOCK_SIZE )
        return size / AES_BLOCK_SIZE + 1;
    else
        return size / AES_BLOCK_SIZE;
}

int CompressAndEncrypt(FILE *src, FILE *dst, unsigned char *key)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    unsigned char iv[] = "0000000000000000";
    char flag[AES_BLOCK_SIZE] = {0};
    unsigned char buffer[CHUNK + AES_BLOCK_SIZE];
    unsigned char cipher[CHUNK + AES_BLOCK_SIZE];
    unsigned blockNum, bufLength;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    while (1)
    {
        strm.avail_in = fread(in, 1, CHUNK, src);
        if (ferror(src))
        {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(src) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        while (1)
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            have = CHUNK - strm.avail_out;

            if(have != 0)
            {
                //Encrypt start
                blockNum = getBlockNum(have);
                sprintf(flag, "%x", have); //get length
                bufLength = AES_BLOCK_SIZE * blockNum + AES_BLOCK_SIZE; //cipher text + flag
                memcpy(buffer, flag, AES_BLOCK_SIZE);
                memcpy(buffer + AES_BLOCK_SIZE, out, AES_BLOCK_SIZE * blockNum);
                encrypt_cbc(buffer, cipher, bufLength, key, iv);
                //Encrypt End

                if (fwrite(cipher, 1, bufLength, dst) != bufLength || ferror(dst))
                {
                    (void)deflateEnd(&strm);
                    return Z_ERRNO;
                }
            }

            if(strm.avail_out != 0)
                break;
        }

        if(flush == Z_FINISH)
            break;
    }

    (void)deflateEnd(&strm);
    return Z_OK;
}

int UncompressAndDecrypt(FILE *src, FILE *dst, unsigned char *key)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char out[CHUNK];

    unsigned char iv[] = "0000000000000000";
    char flag[AES_BLOCK_SIZE] = {0};
    unsigned char cipherFlag[AES_BLOCK_SIZE] = {0};
    unsigned char buffer[CHUNK];
    unsigned char cipher[CHUNK];
    unsigned blockNum, blockLength, bufLength;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    while (1)
    {
        //Read Flag
        if(fread(cipherFlag, 1, AES_BLOCK_SIZE, src) != AES_BLOCK_SIZE)
            break;

        //Decrypt Flag
        ret = decrypt_cbc(cipherFlag, (unsigned char *)flag, AES_BLOCK_SIZE, key, iv);
        if(ret < 0)
        {
            printf("Decrypt Error code : %d \n",ret);
            return -1;
        }
        sscanf(flag, "%x", &bufLength); //get length

        blockNum = getBlockNum(bufLength);
        blockLength = blockNum * AES_BLOCK_SIZE;
        fread(cipher, 1, blockLength, src); //Read Data

        ret = decrypt_cbc(cipher, buffer, bufLength, key, iv);
        if(ret < 0)
        {
            printf("Decrypt Error code : %d \n",ret);
            return -1;
        }

        strm.avail_in = bufLength;
        if (ferror(src))
        {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;

        strm.next_in = buffer;

        while (1)
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret)
            {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dst) != have || ferror(dst))
            {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
            if(strm.avail_out != 0)
                break;
        }

        if(ret == Z_FINISH)
            break;
    }

    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
