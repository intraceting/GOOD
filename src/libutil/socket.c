/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "socket.h"

int good_gethostbyname(const char *name, sa_family_t family, good_sockaddr_t *addrs, int max, char **canonname)
{
    struct addrinfo *results = NULL;
    struct addrinfo *it = NULL;
    struct addrinfo hint = {0};
    int chk;
    int count = 0;

    assert(name != NULL && (family == GOOD_IPV4 || family == GOOD_IPV6) && addrs != NULL && max > 0);

    hint.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
    hint.ai_family = family;

    chk = getaddrinfo(name, NULL, &hint, &results);
    if (chk != 0 || results == NULL)
        return -1;

    for (it = results; it != NULL && count < max; it = it->ai_next)
    {
        if (it->ai_socktype != SOCK_STREAM || it->ai_protocol != IPPROTO_TCP)
            continue;

        if (it->ai_addr->sa_family != GOOD_IPV4 && it->ai_addr->sa_family != GOOD_IPV6)
            continue;

        memcpy(&addrs[count++], it->ai_addr, it->ai_addrlen);
    }

    if (canonname && results->ai_canonname)
        *canonname = good_heap_clone(results->ai_canonname, strlen(results->ai_canonname) + 1);

    freeaddrinfo(results);

    return count;
}

int good_inet_pton(const char *name, sa_family_t family, good_sockaddr_t *addr)
{
    assert(name != NULL && (family == GOOD_IPV4 || family == GOOD_IPV6) && addr != NULL);

    /*
     * bind family
    */
    addr->family = family;

    if (addr->family == GOOD_IPV4)
        return inet_pton(family, name, &addr->addr4.sin_addr);
    if (addr->family == GOOD_IPV6)
        return inet_pton(family, name, &addr->addr6.sin6_addr);

    return -1;
}

char *good_inet_ntop(good_sockaddr_t *addr, char *name, size_t max)
{
    assert(addr != NULL && name != NULL && max > 0);
    assert(addr->family == GOOD_IPV4 || addr->family == GOOD_IPV6);
    assert((addr->family == GOOD_IPV4) ? (max >= INET_ADDRSTRLEN) : 1);
    assert((addr->family == GOOD_IPV6) ? (max >= INET6_ADDRSTRLEN) : 1);

    if (addr->family == GOOD_IPV4)
        return (char *)inet_ntop(addr->family, &addr->addr4.sin_addr, name, max);
    if (addr->family == GOOD_IPV6)
        return (char *)inet_ntop(addr->family, &addr->addr6.sin6_addr, name, max);

    return NULL;
}

int good_ifname_fetch(good_ifaddrs_t *addrs, int max, int ex_loopback)
{
    struct ifaddrs *results = NULL;
    struct ifaddrs *it = NULL;
    good_ifaddrs_t *p = NULL;
    int chk;
    int count = 0;

    assert(addrs != NULL && max > 0);

    chk = getifaddrs(&results);
    if (chk != 0 || results == NULL)
        return -1;

    for (it = results; it != NULL && count < max; it = it->ifa_next)
    {
        if (it->ifa_addr == NULL)
            continue;

        if (it->ifa_addr->sa_family != GOOD_IPV4 && it->ifa_addr->sa_family != GOOD_IPV6)
            continue;

        if ((it->ifa_flags & IFF_LOOPBACK) && ex_loopback)
            continue;

        p = &addrs[count++];

        strncpy(p->name, it->ifa_name, IFNAMSIZ);

        if (GOOD_IPV4 == it->ifa_addr->sa_family)
        {
            memcpy(&p->addr, it->ifa_addr, sizeof(struct sockaddr_in));
            memcpy(&p->mark, it->ifa_netmask, sizeof(struct sockaddr_in));

            if (it->ifa_flags & IFF_BROADCAST)
                memcpy(&p->broa, it->ifa_broadaddr, sizeof(struct sockaddr_in));
            else
                p->broa.family = PF_UNSPEC;
        }
        else if (GOOD_IPV6 == it->ifa_addr->sa_family)
        {
            memcpy(&p->addr, it->ifa_addr, sizeof(struct sockaddr_in6));
            memcpy(&p->mark, it->ifa_netmask, sizeof(struct sockaddr_in6));

            /*
             * IPv6 not support.
            */
            p->broa.family = PF_UNSPEC;
        }
    }

    freeifaddrs(results);

    return count;
}

int good_socket_ioctl(uint32_t cmd, void *args)
{
    int sock = -1;
    int chk;

    assert(cmd != 0 && args != NULL);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        return -1;

    chk = ioctl(sock, cmd, args);

final:

    good_closep(&sock);

    return chk;
}

char *good_mac_fetch(const char *ifname, char addr[12])
{
    struct ifreq args = {0};
    int chk;

    assert(ifname != NULL && addr != NULL);

    strncpy(args.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);

    chk = good_socket_ioctl(SIOCGIFHWADDR, &args);
    if (chk == -1)
        return NULL;

    for (int i = 0; i < 6; i++)
        sprintf(addr + 2 * i, "%02X", (uint8_t)args.ifr_hwaddr.sa_data[i]);

    return addr;
}

int good_getsockopt_int(int fd,int level, int name,int *flag)
{
    socklen_t flen = sizeof(int);

    assert(fd >= 0 && flag != NULL);

    return getsockopt(fd,level,name,flag,&flen);
}

int good_setsockopt_int(int fd,int level, int name,int flag)
{
    socklen_t flen = sizeof(int);

    assert(fd >= 0);

    return setsockopt(fd,level,name,&flag,flen);
}

int good_socket(sa_family_t family, int udp)
{
    int type = SOCK_CLOEXEC;

    assert(family == GOOD_IPV4 || family == GOOD_IPV6);

    type |= (udp ? SOCK_DGRAM : SOCK_STREAM);

    return socket(family, type, 0);
}

int good_bind(int fd, const good_sockaddr_t *addr)
{
    assert(fd >= 0 && addr != NULL);

    return bind(fd, &addr->addr, sizeof(good_sockaddr_t));
}

int good_accept(int fd, good_sockaddr_t *addr)
{
    int sub_fd = -1;
    socklen_t addrlen = sizeof(good_sockaddr_t);

    assert(fd >= 0);

    if (addr)
        sub_fd = accept(fd, &addr->addr, &addrlen);
    else
        sub_fd = accept(fd, NULL, NULL);

    if (sub_fd < 0)
        return -1;

    /*
     * 添加个非必要标志，忽略可能的出错信息。
    */
    good_fflag_add(sub_fd, O_CLOEXEC);

    return sub_fd;
}

int good_connect(int fd, good_sockaddr_t *addr, time_t timeout)
{
    int flags = 0;
    int eno = 0;
    int chk;

    assert(fd >= 0 && addr != NULL);

    flags = good_fflag_get(fd);
    if (flags == -1)
        return -1;

    /*
     * 添加非阻塞标志，用于异步连接。
    */
    chk = 0;
    if (!(flags & O_NONBLOCK))
        chk = good_fflag_add(fd, O_NONBLOCK);

    if (chk != 0)
        return -1;

    chk = connect(fd, &addr->addr, sizeof(good_sockaddr_t));
    if(chk == 0)
        goto final;

    if (errno != EINPROGRESS && errno != EWOULDBLOCK && errno != EAGAIN)
        goto final;

    /*
     * 等待写事件(允许)。
    */
    chk = (good_poll(fd, 0x02, timeout) > 0 ? 0 : -1);
    if(chk != 0)
        goto final;

    /*
     * 获取SOCKET句柄的出错码。
    */
    chk = good_getsockopt_int(fd, SOL_SOCKET, SO_ERROR, &eno);
    chk = (eno == 0 ? 0 : -1);

final:
    
    /*
     * 恢复原有的标志，忽略可能的出错信息。
    */
    if (!(flags & O_NONBLOCK))
        good_fflag_del(fd, O_NONBLOCK);

    return chk;
}