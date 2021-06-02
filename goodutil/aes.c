/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "aes.h"

size_t good_aes_set_key(AES_KEY *key, const void *pwd, size_t len, uint8_t padding, int encrypt)
{
    uint8_t key_buf[32] = {0};
    size_t key_bits = 0;
    int chk;

    assert(key != NULL && pwd != NULL && len > 0);

    if (len <= 16)
    {
        memset(key_buf, padding, 16);
        memcpy(key_buf, pwd, len);
        key_bits = 128;
    }
    else if (len <= 24)
    {
        memset(key_buf, padding, 24);
        memcpy(key_buf, pwd, len);
        key_bits = 192;
    }
    else if (len <= 32)
    {
        memset(key_buf, padding, 32);
        memcpy(key_buf, pwd, len);
        key_bits = 256;
    }

    if (key_bits)
    {
        if (encrypt)
            chk = AES_set_encrypt_key(key_buf, key_bits, key);
        else
            chk = AES_set_decrypt_key(key_buf, key_bits, key);

        assert(chk == 0);
    }

    return key_bits;
}

size_t good_aes_set_iv(uint8_t *iv, const void *salt, size_t len, uint8_t padding)
{
    size_t iv_bytes = 0;

    assert(iv != NULL && salt != NULL && len > 0);

    if (len <= AES_BLOCK_SIZE)
    {
        memset(iv, padding, AES_BLOCK_SIZE);
        memcpy(iv, salt,len);
        iv_bytes = AES_BLOCK_SIZE;
    }
    else if (len <= AES_BLOCK_SIZE * 2)
    {
        memset(iv, padding, AES_BLOCK_SIZE * 2);
        memcpy(iv, salt, len);
        iv_bytes = AES_BLOCK_SIZE * 2;
    }
    else if (len <= AES_BLOCK_SIZE * 4)
    {
        memset(iv, padding, AES_BLOCK_SIZE * 4);
        memcpy(iv, salt, len);
        iv_bytes = AES_BLOCK_SIZE * 4;
    }

    return iv_bytes;
}
