/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "openssl.h"

/******************************************************************************************************/

#ifdef HEADER_AES_H

size_t good_openssl_aes_set_key(AES_KEY *key, const void *pwd, size_t len, uint8_t padding, int encrypt)
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

size_t good_openssl_aes_set_iv(uint8_t *iv, const void *salt, size_t len, uint8_t padding)
{
    size_t iv_bytes = 0;

    assert(iv != NULL && salt != NULL && len > 0);

    if (len <= AES_BLOCK_SIZE)
    {
        memset(iv, padding, AES_BLOCK_SIZE);
        memcpy(iv, salt, len);
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

#endif //HEADER_AES_H

/******************************************************************************************************/

#ifdef HEADER_RSA_H

int good_openssl_rsa_padding_size(int padding)
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

RSA *good_openssl_rsa_create(int bits, unsigned long e)
{
    RSA *key = NULL;

#ifndef OPENSSL_NO_DEPRECATED
    key = RSA_generate_key(bits, e, NULL, NULL);
    if (!key)
        GOOD_ERRNO_AND_RETURN1(EPERM, NULL);
#else
    key = NULL;
#endif

    return key;
}

RSA *good_openssl_rsa_from_fp(FILE *fp, int type, const char *pwd)
{
    RSA *key = NULL;

    assert(fp != NULL);

    if (type)
        key = PEM_read_RSAPrivateKey(fp, NULL, NULL, (void *)pwd);
    else
        key = PEM_read_RSAPublicKey(fp, NULL, NULL, (void *)pwd);

    return key;
}

RSA *good_openssl_rsa_from_file(const char *file, int type, const char *pwd)
{
    FILE *fp;
    RSA *key = NULL;

    assert(file != NULL);

    fp = fopen(file, "r");
    if (!fp)
        GOOD_ERRNO_AND_RETURN1(errno, NULL);

    key = good_openssl_rsa_from_fp(fp, type, pwd);

    /*不要忘记关闭。*/
    fclose(fp);

    return key;
}

int good_openssl_rsa_to_fp(FILE *fp, RSA *key, int type, const char *pwd)
{
    int chk;

    assert(fp != NULL && key != NULL);

    if (type)
        chk = PEM_write_RSAPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL);
    else
        chk = PEM_write_RSAPublicKey(fp, key);

    return chk;
}

int good_openssl_rsa_to_file(const char *file, RSA *key, int type, const char *pwd)
{
    FILE *fp;
    int chk;

    assert(file != NULL && key != NULL);

    fp = fopen(file, "w");
    if (!fp)
        GOOD_ERRNO_AND_RETURN1(errno, -1);

    chk = good_openssl_rsa_to_fp(fp, key, type, pwd);

    /*不要忘记关闭。*/
    fclose(fp);

    return chk;
}

int good_openssl_rsa_encrypt(void *dst, const void *src, int len, RSA *key, int type, int padding)
{
    int chk;

    assert(dst != NULL && src != NULL && len > 0 && key != NULL);

    if (type)
        chk = RSA_private_encrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);
    else
        chk = RSA_public_encrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);

    return chk;
}

int good_openssl_rsa_decrypt(void *dst, const void *src, int len, RSA *key, int type, int padding)
{
    int chk;

    assert(dst != NULL && src != NULL && len > 0 && key != NULL);

    if (type)
        chk = RSA_private_decrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);
    else
        chk = RSA_public_decrypt(len, (uint8_t *)src, (uint8_t *)dst, key, padding);

    return chk;
}

ssize_t good_openssl_rsa_ecb_encrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding)
{
    int key_size;
    size_t padding_size;
    size_t block_size;
    size_t blocks;
    size_t fixlen;
    void *fix_buf = NULL;
    ssize_t ret = -1;
    int chk;

    assert(len > 0 && key != NULL);

    key_size = RSA_size(key);
    padding_size = good_openssl_rsa_padding_size(padding);
    block_size = key_size - padding_size;

    blocks = len / block_size;
    fixlen = len % block_size;

    if (!dst || !src)
        goto final;

    for (size_t i = 0; i < blocks; i++)
    {
        chk = good_openssl_rsa_encrypt(GOOD_PTR2VPTR(dst, i * key_size), GOOD_PTR2VPTR(src, i * block_size),
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

        chk = good_openssl_rsa_encrypt(GOOD_PTR2VPTR(dst, blocks * key_size), fix_buf,
                                       block_size, key, type, padding);

        if (chk <= 0)
            goto final_error;
    }

final:

    ret = blocks * key_size + ((fixlen > 0) ? key_size : 0);

final_error:

    if (fix_buf)
        good_heap_freep(&fix_buf);

    return ret;
}

int good_openssl_rsa_ecb_decrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding)
{
    int key_size;
    size_t padding_size;
    size_t block_size;
    size_t blocks;
    size_t fixlen;
    int chk;

    assert(dst != NULL && src != NULL && len > 0 && key != NULL);

    key_size = RSA_size(key);
    padding_size = good_openssl_rsa_padding_size(padding);
    block_size = key_size - padding_size;

    blocks = len / key_size;
    fixlen = len % key_size;

    if (fixlen != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

    for (size_t i = 0; i < blocks; i++)
    {
        chk = good_openssl_rsa_decrypt(GOOD_PTR2VPTR(dst, i * block_size), GOOD_PTR2VPTR(src, i * key_size),
                                       key_size, key, type, padding);

        if (chk <= 0)
            goto final_error;
    }

    return 1;

final_error:

    return -1;
}

#endif //EADER_RSA_H

/******************************************************************************************************/

#ifdef HEADER_HMAC_H

int good_openssl_hmac_init(HMAC_CTX *hmac, const void *key, int len, int type)
{
    int chk;

    assert(hmac != NULL && key != NULL && len > 0);
    assert(type >= GOOD_OPENSSL_HMAC_MD2 && type <= GOOD_OPENSSL_HMAC_WHIRLPOOL);

    /*不可以省略。*/
    HMAC_CTX_init(hmac);

    if (0)
        assert(0);
#ifndef OPENSSL_NO_MD2
    else if (type == GOOD_OPENSSL_HMAC_MD2)
        chk = HMAC_Init_ex(hmac, key, len, EVP_md2(), NULL);
#endif
#ifndef OPENSSL_NO_MD4
    else if (type == GOOD_OPENSSL_HMAC_MD4)
        chk = HMAC_Init_ex(hmac, key, len, EVP_md4(), NULL);
#endif
#ifndef OPENSSL_NO_MD5
    else if (type == GOOD_OPENSSL_HMAC_MD5)
        chk = HMAC_Init_ex(hmac, key, len, EVP_md5(), NULL);
#endif
#ifndef OPENSSL_NO_SHA
    else if (type == GOOD_OPENSSL_HMAC_SHA)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha(), NULL);
    else if (type == GOOD_OPENSSL_HMAC_SHA1)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha1(), NULL);
#endif
#ifndef OPENSSL_NO_SHA256
    else if (type == GOOD_OPENSSL_HMAC_SHA224)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha224(), NULL);
    else if (type == GOOD_OPENSSL_HMAC_SHA256)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha256(), NULL);
#endif
#ifndef OPENSSL_NO_SHA512
    else if (type == GOOD_OPENSSL_HMAC_SHA384)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha384(), NULL);
    else if (type == GOOD_OPENSSL_HMAC_SHA512)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha512(), NULL);
#endif
#ifndef OPENSSL_NO_RIPEMD
    else if (type == GOOD_OPENSSL_HMAC_RIPEMD160)
        chk = HMAC_Init_ex(hmac, key, len, EVP_ripemd160(), NULL);
#endif
#ifndef OPENSSL_NO_WHIRLPOOL
    else if (type == GOOD_OPENSSL_HMAC_WHIRLPOOL)
        chk = HMAC_Init_ex(hmac, key, len, EVP_whirlpool(), NULL);
#endif

    return (chk == 1) ? 0 : -1;
}

#endif //HEADER_HMAC_H

/******************************************************************************************************/

/******************************************************************************************************/

#ifdef HEADER_SSL_H

void good_openssl_ctx_freep(SSL_CTX **ctx)
{
    if (!ctx || !*ctx)
        return;

    SSL_CTX_free(*ctx);

    /*Set to NULL(0).*/
    *ctx = NULL;
}

static int _good_openssl_ctx_init(void *opaque)
{
    SSL_METHOD **method = (SSL_METHOD **)opaque;

    method[0] = (SSL_METHOD *)TLSv1_2_server_method();
    method[1] = (SSL_METHOD *)TLSv1_2_client_method();

    method[2] = (SSL_METHOD *)TLSv1_1_server_method();
    method[3] = (SSL_METHOD *)TLSv1_1_client_method();

    method[4] = (SSL_METHOD *)TLSv1_server_method();
    method[5] = (SSL_METHOD *)TLSv1_client_method();

#ifndef OPENSSL_NO_SSL3_METHOD
    method[6] = (SSL_METHOD *)SSLv3_server_method();
    method[7] = (SSL_METHOD *)SSLv3_client_method();
#endif //OPENSSL_NO_SSL3_METHOD

    method[8] = (SSL_METHOD *)SSLv23_server_method();
    method[9] = (SSL_METHOD *)SSLv23_client_method();

    return 0;
}

SSL_CTX *good_openssl_ctx_alloc(int version, int server)
{
    static int init = 0;
    static SSL_METHOD *method[10] = {0};
    SSL_METHOD *p;

    assert(good_once(&init, _good_openssl_ctx_init, &method) >= 0);

    if (version == 12)
        p = server ? method[0] : method[1];
    else if (version == 11)
        p = server ? method[2] : method[3];
    else if (version == 10)
        p = server ? method[4] : method[5];
    else if (version == 30)
        p = server ? method[6] : method[7];
    else if (version == 23)
        p = server ? method[8] : method[9];

    return SSL_CTX_new(p);
}

int good_openssl_ctx_load_cert(SSL_CTX *ctx, const char *cert, const char *key, const char *pwd)
{
    int chk;

    assert(ctx != NULL);

    if (cert)
    {
        chk = SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);
    }

    if (key && pwd)
        SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)pwd);

    if (key)
    {
        chk = SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);
    }

    if (cert && key)
    {
        chk = SSL_CTX_check_private_key(ctx);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);
    }

    return 0;

final_error:

    return -1;
}

void good_openssl_ssl_freep(SSL **ssl)
{
    if (!ssl || !*ssl)
        return;

    SSL_shutdown(*ssl);
    SSL_free(*ssl);

    /*Set to NULL(0).*/
    *ssl = NULL;
}

SSL *good_openssl_ssl_alloc(SSL_CTX *ctx)
{
    assert(ctx != NULL);

    return SSL_new(ctx);
}

int good_openssl_ssl_handshake(int fd, SSL *ssl, int server, time_t timeout)
{
    int err;
    int chk;

    assert(fd >= 0 && ssl != NULL);

    if (SSL_get_fd(ssl) != fd)
    {
        chk = SSL_set_fd(ssl, fd);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

        if (server)
            SSL_set_accept_state(ssl);
        else
            SSL_set_connect_state(ssl);
    }

try_again:

    chk = SSL_do_handshake(ssl);
    if (chk == 1)
        goto final;

    err = SSL_get_error(ssl, chk);
    if (err == SSL_ERROR_WANT_WRITE)
    {
        chk = good_poll(fd, 0x02, timeout);
        if (chk <= 0)
            GOOD_ERRNO_AND_GOTO1(ETIMEDOUT, final_error);

        GOOD_ERRNO_AND_GOTO1(0, try_again);
    }
    else if (err == SSL_ERROR_WANT_READ)
    {
        chk = good_poll(fd, 0x01, timeout);
        if (chk <= 0)
            GOOD_ERRNO_AND_GOTO1(ETIMEDOUT, final_error);

        GOOD_ERRNO_AND_GOTO1(0, try_again);
    }
    else
    {
        GOOD_ERRNO_AND_GOTO1(ENOSYS, final_error);
    }

final:

    return 0;

final_error:

    return -1;
}

#endif //HEADER_SSL_H

/******************************************************************************************************/