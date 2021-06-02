/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_AES_H
#define GOODUTIL_AES_H

#include "general.h"

#ifdef HEADER_AES_H

/**
 *
 * 设置密钥
 * 
 * @param pwd 密钥的指针。
 * @param len 密钥的长度(字节），1～32之间，自动对齐到16、24、32三类长度，不足部分用padding补齐。
 * @param padding 填充字符。
 * @param encrypt !0 加密密钥，0 解密密钥。
 * 
 * @return > 0 成功(密钥长度(bits))，<= 0 失败。
 * 
*/
size_t good_aes_set_key(AES_KEY *key, const void *pwd, size_t len, uint8_t padding, int encrypt);

/**
 * 设置向量
 * 
 * @param salt “盐”的指针。
 * @param len “盐”的长度(字节），1～32之间，自动对齐到16、24、32三类长度，不足部分用padding补齐。
 * @param padding 填充字符。
 * 
 * @return > 0 成功(向量长度(字节))，<= 0 失败。
 * 
*/
size_t good_aes_set_iv(uint8_t *iv, const void *salt, size_t len, uint8_t padding);



#endif //HEADER_AES_H

#endif //GOODUTIL_AES_H
