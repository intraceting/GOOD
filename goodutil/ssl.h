/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_SSL_H
#define GOODUTIL_SSL_H

#include "general.h"

#ifdef HEADER_SSL_H

/**
 * 释放CTX句柄。
 * 
*/
void good_ssl_ctx_freep(SSL_CTX **ctx);

/**
 * 创建CTX句柄。
 * 
 * TLSv1.0              = 10
 * TLSv1.1              = 11
 * TLSv1.2              = 12
 * SSLv2/SSLv3          = 23
 * SSLv3                = 30
 * 
 * @param version 版本号。
 * @param server !0 服务端环境，0 客户端环境。
 * 
 * @return !NULL(0) 成功(句柄)，NULL(0) 失败。
*/
SSL_CTX* good_ssl_ctx_alloc(int version,int server);

/**
 * 加载证书、私钥。
 * 
 * @param cert 证书文件的指针。
 * @param key 私钥文件的指针，NULL(0) 忽略。
 * @param pwd 密码的指针，NULL(0) 忽略。
 * 
 * @return 0 成功(句柄)，-1 失败。
*/
int good_ssl_ctx_load_cert(SSL_CTX *ctx,const char *cert,const char *key,const char *pwd);

/**
 * 释放SSL句柄。
*/
void good_ssl_freep(SSL **ssl);

/**
 * 创建SSL句柄。
 * 
 * @return !NULL(0) 成功(句柄)，NULL(0) 失败。
*/
SSL *good_ssl_alloc(SSL_CTX *ctx);

/**
 * 握手。
 * 
 * @param fd 文件或SOCKET句柄。
 * @param timeout 超时(毫秒)。
 * 
 * @return 0 成功(句柄)，-1 失败。
*/
int good_ssl_handshake(int fd, SSL *ssl, int server, time_t timeout);

#endif //HEADER_SSL_H

#endif //GOODUTIL_SSL_H