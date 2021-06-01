/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_RSA_H
#define GOODUTIL_RSA_H

#include "general.h"

#ifdef HEADER_RSA_H

/**
 * 计算补充数据(盐)长度(字节)。
 * 
 * @param padding 见RSA_*_PADDING。
 * 
 * @return >=0 成功(长度)，< 0 失败。
*/
int good_rsa_padding_size(int padding);

/**
 * 创建RSA密钥对象。
 * 
 * @param bits KEY的长度(bits)
 * @param e 指数，见RSA_3/RSA_F4
 * 
 * @return !NULL(0) 成功(对象的指针)，NULL(0) 失败。
*/
RSA *good_rsa_create(int bits, unsigned long e);

/**
 * 从文件导入密钥，并创建密钥对象。
 * 
 * @param type !0 私钥，0 公钥。
 * @param pwd 密钥密码的指针，NULL(0)忽略。
 * 
 * @return !NULL(0) 成功(对象的指针)，NULL(0) 失败。
*/
RSA *good_rsa_from_fp(FILE *fp,int type,const char *pwd);

/**
 * 从文件导入密钥，并创建密钥对象。
 * 
 * @return !NULL(0) 成功(对象的指针)，NULL(0) 失败。
*/
RSA *good_rsa_from_file(const char *file,int type,const char *pwd);

/**
 * 向文件导出密钥。
 * 
 * @param pwd 密钥密码的指针，NULL(0)忽略。私钥有效。
 * 
 * @return > 0 成功，<= 0 失败。
*/
int good_rsa_to_fp(FILE *fp, RSA *key, int type, const char *pwd);

/**
 * 向文件导出密钥。
 * 
 * @return > 0 成功，<= 0 失败。
*/
int good_rsa_to_file(const char *file, RSA *key, int type, const char *pwd);

/**
 * 加密。
 * 
 * @param dst 密文的指针。
 * @param src 明文的指针。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功，<= 0 失败。
 * 
 */
int good_rsa_encrypt(void *dst, const void *src, int len, RSA *key, int type, int padding);

/**
 * 解密。
 * 
 * @param dst 明文的指针。
 * @param src 密文的指针。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功，<= 0 失败。
 * 
 */
int good_rsa_decrypt(void *dst, const void *src, int len, RSA *key, int type, int padding);

/**
 * 加密，ECB模式。
 * 
 * @param dst 密文的指针，NULL(0) 计算密文的长度。
 * @param src 明文的指针，NULL(0) 计算密文的长度。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功(密文的长度)，<= 0 失败。
 * 
 */
ssize_t good_rsa_ecb_encrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding);

/**
 * 解密，ECB模式。
 * 
 * @param dst 明文的指针，可用空间至少与密文长度的相等。
 * @param src 密文的指针。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功，<= 0 失败。
 */
int good_rsa_ecb_decrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding);


#endif //EADER_RSA_H

#endif //GOODUTIL_RSA_H
