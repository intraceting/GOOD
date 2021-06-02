/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "hmac.h"

int good_hmac_init(HMAC_CTX *hmac, const void *key, int len, int type)
{
    int chk;

    assert(hmac != NULL && key != NULL && len > 0);
    assert(type >= GOOD_HMAC_MD2 && type <= GOOD_HMAC_WHIRLPOOL);

    /*不可以省略。*/
    HMAC_CTX_init(hmac);

    if (0)
        assert(0);
#ifndef OPENSSL_NO_MD2
    else if (type == GOOD_HMAC_MD2)
        chk = HMAC_Init_ex(hmac, key, len, EVP_md2(), NULL);
#endif
#ifndef OPENSSL_NO_MD4
    else if (type == GOOD_HMAC_MD4)
        chk = HMAC_Init_ex(hmac, key, len, EVP_md4(), NULL);
#endif
#ifndef OPENSSL_NO_MD5
    else if (type == GOOD_HMAC_MD5)
        chk = HMAC_Init_ex(hmac, key, len, EVP_md5(), NULL);
#endif
#ifndef OPENSSL_NO_SHA
    else if (type == GOOD_HMAC_SHA)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha(), NULL);
    else if (type == GOOD_HMAC_SHA1)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha1(), NULL);
#endif
#ifndef OPENSSL_NO_SHA256
    else if (type == GOOD_HMAC_SHA224)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha224(), NULL);
    else if (type == GOOD_HMAC_SHA256)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha256(), NULL);
#endif
#ifndef OPENSSL_NO_SHA512
    else if (type == GOOD_HMAC_SHA384)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha384(), NULL);
    else if (type == GOOD_HMAC_SHA512)
        chk = HMAC_Init_ex(hmac, key, len, EVP_sha512(), NULL);
#endif
#ifndef OPENSSL_NO_RIPEMD
    else if (type == GOOD_HMAC_RIPEMD160)
        chk = HMAC_Init_ex(hmac, key, len, EVP_ripemd160(), NULL);
#endif
#ifndef OPENSSL_NO_WHIRLPOOL
    else if (type == GOOD_HMAC_WHIRLPOOL)
        chk = HMAC_Init_ex(hmac, key, len, EVP_whirlpool(), NULL);
#endif

    return (chk == 1) ? 0 : -1;
}