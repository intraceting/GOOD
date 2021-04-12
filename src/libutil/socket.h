/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_SOCKET_H
#define GOOD_UTIL_SOCKET_H

#include "general.h"

/**
 * IPv4
*/
#define GOOD_IPV4   AF_INET

/**
 * IPv6
*/
#define GOOD_IPV6   AF_INET6

/**
 * Socket Address
*/
typedef union _good_sockaddr
{
    sa_family_t family;
    struct sockaddr addr;
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
} good_sockaddr_t;

/*
 * Interface Name
 */
typedef struct _good_ifname
{
    char name[IF_NAMESIZE];
    good_sockaddr_t addr;
    good_sockaddr_t mark;
    good_sockaddr_t broa;
} good_ifname_t;

/**
 * 域名解析。
 * 
 * @param addrs IP地址数组的指针。
 * @param max IP地址数组元素最大数量。
 * @param canonname 规范名称指针的指针，NULL(0)忽略。返回的指针需要用good_heap_free()释放。
 *
 * @return >= 0 IP地址数量， < 0 出错。
 * 
 * @see getaddrinfo()
 * @see freeaddrinfo()
 * @see good_heap_alloc()
*/
int good_gethostbyname(const char* name,sa_family_t family,good_sockaddr_t *addrs,int max,char** canonname);

/**
 * IP字符串转IP地址。
 *
 * @return 1 成功，<= 0 失败。
 * 
 * @see inet_pton()
*/
int good_inet_pton(const char* name,sa_family_t family,good_sockaddr_t *addr);

/**
 * IP地址转IP字符串。
 *
 * @return !NULL(0) IP字符串指针，NULL(0) 失败。
 * 
 * @see inet_ntop()
*/
char* good_inet_ntop(good_sockaddr_t *addr,char* name,size_t max);

/**
 * 获取网络接口信息
 * 
 * @param ex_loopback 0 包括回环接口，!0 排除回环信接口。
 * 
 * @return >= 0 网络接口数量， < 0 出错。
*/
int good_ifname_fetch(good_ifname_t *ifnames,int max,int ex_loopback);

#endif //GOOD_UTIL_SOCKET_H