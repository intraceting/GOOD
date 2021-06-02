/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_HMAC_H
#define GOODUTIL_HMAC_H

#include "general.h"

#ifdef HEADER_HMAC_H

/**
 * HMAC支持的算法。
*/
enum _good_hmac_type
{
    GOOD_HMAC_MD2 = 1,
#define GOOD_HMAC_MD2 GOOD_HMAC_MD2

    GOOD_HMAC_MD4 = 2,
#define GOOD_HMAC_MD4 GOOD_HMAC_MD4

    GOOD_HMAC_MD5 = 3,
#define GOOD_HMAC_MD5 GOOD_HMAC_MD5

    GOOD_HMAC_SHA = 4,
#define GOOD_HMAC_SHA GOOD_HMAC_SHA

    GOOD_HMAC_SHA1 = 5,
#define GOOD_HMAC_SHA1 GOOD_HMAC_SHA1

    GOOD_HMAC_SHA224 = 6,
#define GOOD_HMAC_SHA224 GOOD_HMAC_SHA224

    GOOD_HMAC_SHA256 = 7,
#define GOOD_HMAC_SHA256 GOOD_HMAC_SHA256

    GOOD_HMAC_SHA384 = 8,
#define GOOD_HMAC_SHA384 GOOD_HMAC_SHA384

    GOOD_HMAC_SHA512 = 9,
#define GOOD_HMAC_SHA512 GOOD_HMAC_SHA512

    GOOD_HMAC_RIPEMD160 = 10,
#define GOOD_HMAC_RIPEMD160 GOOD_HMAC_RIPEMD160

    GOOD_HMAC_WHIRLPOOL = 11
#define GOOD_HMAC_WHIRLPOOL GOOD_HMAC_WHIRLPOOL
};

/**
 * 初始化环境。
 * 
 *  @return 0 成功，!0 失败。
*/
int good_hmac_init(HMAC_CTX *hmac,const void *key, int len,int type);


#endif //HEADER_HMAC_H

#endif //GOODUTIL_HMAC_H