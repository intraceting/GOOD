/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "rsa.h"


#ifdef HEADER_RSA_H

int good_rsa_padding_size(int padding)
{
    int size = -1;

    switch (padding)
    {
    case RSA_PKCS1_PADDING:
        size = RSA_PKCS1_PADDING_SIZE; //=11
        break;
    case RSA_PKCS1_OAEP_PADDING:
        size = 42;
        break;
    case RSA_NO_PADDING:
    default:
        size = 0;
        break;
    }

    return size;
}

RSA *good_rsa_create(int bits, unsigned long e)
{
    RSA* key = NULL;

#ifndef OPENSSL_NO_DEPRECATED
    key = RSA_generate_key(bits,e, NULL, NULL);
    if (!key)
        GOOD_ERRNO_AND_RETURN1(EPERM,NULL);
#else
    key = NULL;
#endif

    return key;
}

RSA *good_rsa_from_fp(FILE *fp,int type,const char *pwd)
{
    RSA *key = NULL;

    assert(fp != NULL);

    if (type)
        key = PEM_read_RSAPrivateKey(fp, NULL, NULL, (void*)pwd);
    else
        key = PEM_read_RSAPublicKey(fp, NULL, NULL, (void*)pwd);

    return key;
}

RSA *good_rsa_from_file(const char *file,int type,const char *pwd)
{
    FILE *fp;
    RSA *key = NULL;

    assert(file != NULL);

    fp = fopen(file,"r");
    if(!fp)
        GOOD_ERRNO_AND_RETURN1(errno,NULL);

    key = good_rsa_from_fp(fp,type,pwd);

    /*不要忘记关闭。*/
    fclose(fp);

    return key;
}

int good_rsa_to_fp(FILE *fp, RSA *key, int type, const char *pwd)
{
    int chk;

    assert(fp != NULL && key != NULL);

    if(type)
        chk = PEM_write_RSAPrivateKey(fp,key, NULL, NULL, 0, NULL, NULL);
    else
        chk = PEM_write_RSAPublicKey(fp,key);

    return chk;
}

int good_rsa_to_file(const char *file, RSA *key, int type, const char *pwd)
{
    FILE *fp;
    int chk;

    assert(file != NULL && key != NULL);

    fp = fopen(file,"w");
    if(!fp)
        GOOD_ERRNO_AND_RETURN1(errno,-1);

    chk = good_rsa_to_fp(fp,key,type,pwd);

    /*不要忘记关闭。*/
    fclose(fp);

    return chk;
}

int good_rsa_encrypt(void *dst, const void *src, int len, RSA *key, int type, int padding)
{
    int chk;

    assert(dst != NULL && src != NULL && len >0 && key != NULL);

    if (type)
        chk = RSA_private_encrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);
    else
        chk = RSA_public_encrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);

    return chk;
}

int good_rsa_decrypt(void *dst, const void *src, int len, RSA *key, int type, int padding)
{
    int chk;

    assert(dst != NULL && src != NULL && len >0 && key != NULL);

    if (type)
        chk = RSA_private_decrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);
    else
        chk = RSA_public_decrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);

    return chk;
}

ssize_t good_rsa_ecb_encrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding)
{
    int key_size;
    size_t padding_size;
    size_t block_size;
    size_t blocks;
    size_t fixlen;
    void *fix_buf = NULL;
    ssize_t ret = -1;
    int chk;

    assert(len >0 && key != NULL);

    key_size = RSA_size(key);
    padding_size = good_rsa_padding_size(padding);
    block_size = key_size - padding_size;

    blocks = len / block_size;
    fixlen = len % block_size;

    if (!dst || !src)
        goto final;

    for (size_t i = 0; i < blocks; i++)
    {
        chk = good_rsa_encrypt(GOOD_PTR2VPTR(dst, i * key_size), GOOD_PTR2VPTR(src, i * block_size),
                               block_size, key, type, padding);

        if (chk <= 0)
            goto final_error;
    }

    if (fixlen > 0)
    {
        fix_buf = good_heap_alloc(block_size);
        if (!fix_buf)
            goto final_error;

        memcpy(fix_buf, GOOD_PTR2VPTR(src, blocks * block_size), fixlen);

        chk = good_rsa_encrypt(GOOD_PTR2VPTR(dst, blocks * key_size), fix_buf,
                               block_size, key, type, padding);

        if (chk <= 0)
            goto final_error;
    }

final:

    ret = blocks * key_size + ((fixlen > 0) ? key_size : 0);

final_error:

    if(fix_buf)
        good_heap_freep(&fix_buf);

    return ret;
}

int good_rsa_ecb_decrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding)
{
    int key_size;
    size_t padding_size;
    size_t block_size;
    size_t blocks;
    size_t fixlen;
    int chk;

    assert(dst != NULL && src != NULL && len >0 && key != NULL);

    key_size = RSA_size(key);
    padding_size = good_rsa_padding_size(padding);
    block_size = key_size - padding_size;

    blocks = len / key_size;
    fixlen = len % key_size;

    if(fixlen != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL,final_error);

    for (size_t i = 0; i < blocks; i++)
    {
        chk = good_rsa_decrypt(GOOD_PTR2VPTR(dst, i * block_size), GOOD_PTR2VPTR(src, i * key_size),
                               key_size, key, type, padding);

        if (chk <= 0)
            goto final_error;
    }

    return 1;

final_error:

    return -1;
}

#endif //EADER_RSA_H