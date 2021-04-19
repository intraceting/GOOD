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
#define GOOD_IPV4 AF_INET

/**
 * IPv6
*/
#define GOOD_IPV6 AF_INET6

/**
 * Socket地址
*/
typedef union _good_sockaddr
{
    /**
     * 协议。
    */
    sa_family_t family;

    /**
     * 通用的地址。
    */
    struct sockaddr addr;

    /**
     * IPv4地址。
    */
    struct sockaddr_in addr4;

    /**
     * IPv6地址。
    */
    struct sockaddr_in6 addr6;
} good_sockaddr_t;

/*
 * 网卡接口地址。
 */
typedef struct _good_ifaddrs
{
    /**
     * 接口名称
    */
    char name[IF_NAMESIZE];

    /**
     * 接口地址。
     * 
    */
    good_sockaddr_t addr;

    /**
     * 掩码地址。
     * 
    */
    good_sockaddr_t mark;

    /**
     * 广播地址。
     * 
     * @warning IPv6无效。
    */
    good_sockaddr_t broa;
} good_ifaddrs_t;

/**
 * 域名解析。
 * 
 * @param addrs IP地址数组的指针。
 * @param max IP地址数组元素最大数量。
 * @param canonname 规范名称指针的指针，NULL(0)忽略。返回的指针需要用good_heap_free()释放。
 *
 * @return >= 0 IP地址数量， < 0 出错。
*/
int good_gethostbyname(const char *name, sa_family_t family, good_sockaddr_t *addrs, int max, char **canonname);

/**
 * IP字符串转IP地址。
 *
 * @return 1 成功，<= 0 失败。
*/
int good_inet_pton(const char *name, sa_family_t family, good_sockaddr_t *addr);

/**
 * IP地址转IP字符串。
 *
 * @return !NULL(0) IP字符串指针，NULL(0) 失败。
*/
char *good_inet_ntop(good_sockaddr_t *addr, char *name, size_t max);

/**
 * 获取网络接口信息
 * 
 * @param ex_loopback 0 包括回环接口，!0 排除回环接口。
 * 
 * @return >= 0 网络接口数量， < 0 出错。
*/
int good_ifname_fetch(good_ifaddrs_t *addrs, int max, int ex_loopback);

/**
 * SOCKET IO control
 * 
 * @return !-1 成功，-1 失败。
*/
int good_socket_ioctl(uint32_t cmd, void *args);

/**
 * 查询网卡地址。
 * 
 * 格式化为十六进制字符串。
 * 
 * @return !NULL(0) 成功(网卡地址字符串的指针)，NULL(0) 失败。
 * 
*/
char *good_mac_fetch(const char *ifname, char addr[12]);

/**
 * 获取或设置SOCKET选项。
 * 
 * @param direction 方向。 1 读，2 写。
 * 
 * @return 0 成功，-1 失败。
*/
int good_socket_option(int fd, int level, int name, void *data, int *len, int direction);

/**
 * 获取或设置SOCKET选项(integer)。
 * 
 * @param direction 方向。 1 读，2 写。
 * 
 * @return 0 成功，-1 失败。
*/
int good_sockopt_option_int(int fd, int level, int name, int *flag, int direction);

/**
 * 获取或设置SOCKET选项(timeout)。
 * 
 * @param direction 方向。 1 读，2 写。
 * 
 * @return 0 成功，-1 失败。
*/
int good_sockopt_option_timeout(int fd, int name, struct timeval *tv, int direction);

/**
 * 获取或设置SOCKET选项(linger)。
 * 
 * @param direction 方向。 1 读，2 写。
 * 
 * @return 0 成功，-1 失败。
*/
int good_socket_option_linger(int fd, struct linger *lg, int direction);

/**
 * 获取或设置SOCKET选项(multicast)。
 * 
 * @param direction 方向。 1 读，2 写。
 * 
 * @return 0 成功，-1 失败。
*/
int good_socket_option_multicast(int fd, int name, good_sockaddr_t *multiaddr, const char *ifaddr, int direction);

/**
 * 创建一个SOCKET句柄。
 * 
 * @param udp 0 创建TCP句柄，!0 创建UDP句柄。
 * 
 * @return >= 0 成功(SOCKET句柄)，-1 失败。
*/
int good_socket(sa_family_t family, int udp);

/**
 * 绑定地址到SOCKET句柄。
 *
 * @return 0 成功(SOCKET句柄)，!0 失败。
*/
int good_bind(int fd, const good_sockaddr_t *addr);

/**
 * 接收一个已经连接的SOCKET句柄。
 * 
 * @param addr 远程地址的指针，NULL(0) 忽略。
 * 
 * @return >= 0 成功(SOCKET句柄)，-1 失败。
*/
int good_accept(int fd, good_sockaddr_t *addr);

/**
 * 使用已经打开的SOCKET句柄创建到远程地址的连接。
 * 
 * @param timeout 超时(毫秒)。>= 0 连接成功或时间过期，< 0 直到连接成功或出错。
 * 
 * @return 0 成功，-1 失败(或超时)。
 * 
*/
int good_connect(int fd, good_sockaddr_t *addr, time_t timeout);

#endif //GOOD_UTIL_SOCKET_H