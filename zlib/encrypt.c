#include <stdio.h>
#include "encrypt.h"

int encrypt_cbc(unsigned char *in, unsigned char *out, int length, unsigned char *key, unsigned char *ivec)
{
    AES_KEY aes_key;
    if(AES_set_encrypt_key(key, 128, &aes_key) < 0)
    {
        return -1;
    }

    AES_cbc_encrypt(in, out, length, &aes_key, ivec,
           AES_ENCRYPT);

    return 1;
}

int decrypt_cbc(unsigned char *in, unsigned char *out, int length, unsigned char *key, unsigned char *ivec)
{
    AES_KEY aes_key;
    if(AES_set_decrypt_key(key, 128, &aes_key) < 0)
    {
        return -1;
    }

    AES_cbc_encrypt(in, out, length, &aes_key, ivec,
           AES_DECRYPT);

    return 1;
}
