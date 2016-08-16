#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "zlib.h"
#include "compress.h"
#include "encrypt.h"

int getBlockLength(int size)
{
    if( size % AES_BLOCK_SIZE )
        return (size / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    else
        return (size / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
}

int fillBuffer(unsigned char *buffer, int *index, unsigned char *str, int length)
{
    memcpy(buffer + *index, str, length);
    *index += length; //new index
    if( *index < CHUNK )
        return 0; //buffer is not full
    else
        return 1; //buffer is full
}

int compressAndEncrypt(int src, int dst, unsigned char *key)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    unsigned char iv[] = "0000000000000000";
    unsigned char cipher[CHUNK];
    unsigned char buffer[2*CHUNK]; // buffer length = 2 * CHUNK
    unsigned char *block;
    int blockLength;
    int index = 0;

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
        strm.avail_in = read(src, in, CHUNK);
        if ( strm.avail_in == 0 )
            flush = Z_FINISH;
        else
            flush = Z_NO_FLUSH;

        strm.next_in = in;

        while (1)
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            have = CHUNK - strm.avail_out;

            if(have != 0)
            {
                if( fillBuffer(buffer, &index, out, have) ) //buffer is full
                {
                    //Encrypted buffer length = CHUNK
                    encrypt_cbc(buffer, cipher, CHUNK, key, iv);

                    //Write file
                    write(dst, cipher, CHUNK);

                    memcpy(buffer, buffer + CHUNK, CHUNK);
                    index = index - CHUNK;
                }
            }

            if(strm.avail_out != 0)
                break;
        }

        if(flush == Z_FINISH)
        {
            blockLength = getBlockLength(index);
            block = (unsigned char *)malloc(blockLength);

            encrypt_cbc(buffer, block, blockLength, key, iv);
            write(dst, block, blockLength); //Write Last Block

            free(block);
            break;
        }
    }

    (void)deflateEnd(&strm);
    return Z_OK;
}

int uncompressAndDecrypt(int src, int dst, unsigned char *key)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char out[CHUNK];

    unsigned char iv[] = "0000000000000000";
    unsigned char buffer[CHUNK];
    unsigned char cipher[CHUNK];
    int len;

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
        len = read(src, cipher, CHUNK); //Read Data
        if (len == 0)
            break;

        ret = decrypt_cbc(cipher, buffer, len, key, iv);
        if(ret < 0)
        {
            printf("Decrypt Error code : %d \n",ret);
            return -1;
        }

        strm.avail_in = len;
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
            if ( write(dst, out, have) != have )
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
