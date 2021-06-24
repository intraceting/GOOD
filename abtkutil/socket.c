/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "socket.h"

int abtk_gethostbyname(const char *name, sa_family_t family, abtk_sockaddr_t *addrs, int max, char **canonname)
{
    struct addrinfo *results = NULL;
    struct addrinfo *it = NULL;
    struct addrinfo hint = {0};
    int chk;
    int count = 0;

    assert(name != NULL && (family == ABTK_IPV4 || family == ABTK_IPV6) && addrs != NULL && max > 0);

    hint.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
    hint.ai_family = family;

    chk = getaddrinfo(name, NULL, &hint, &results);
    if (chk != 0 || results == NULL)
        return -1;

    for (it = results; it != NULL && count < max; it = it->ai_next)
    {
        if (it->ai_socktype != SOCK_STREAM || it->ai_protocol != IPPROTO_TCP)
            continue;

        if (it->ai_addr->sa_family != ABTK_IPV4 && it->ai_addr->sa_family != ABTK_IPV6)
            continue;

        memcpy(&addrs[count++], it->ai_addr, it->ai_addrlen);
    }

    if (canonname && results->ai_canonname)
        *canonname = abtk_heap_clone(results->ai_canonname, strlen(results->ai_canonname) + 1);

    freeaddrinfo(results);

    return count;
}

int abtk_inet_pton(const char *name, sa_family_t family, abtk_sockaddr_t *addr)
{
    int chk = -1;

    assert(name != NULL && (family == ABTK_IPV4 || family == ABTK_IPV6) && addr != NULL);

    /*bind family*/
    addr->family = family;

    if (addr->family == ABTK_IPV4)
        chk = (inet_pton(family, name, &addr->addr4.sin_addr) == 1 ? 0 : -1);
    if (addr->family == ABTK_IPV6)
        chk = (inet_pton(family, name, &addr->addr6.sin6_addr) == 1 ? 0 : -1);

    return chk;
}

char *abtk_inet_ntop(const abtk_sockaddr_t *addr, char *name, size_t max)
{
    assert(addr != NULL && name != NULL && max > 0);
    assert(addr->family == ABTK_IPV4 || addr->family == ABTK_IPV6);
    assert((addr->family == ABTK_IPV4) ? (max >= INET_ADDRSTRLEN) : 1);
    assert((addr->family == ABTK_IPV6) ? (max >= INET6_ADDRSTRLEN) : 1);

    if (addr->family == ABTK_IPV4)
        return (char *)inet_ntop(addr->family, &addr->addr4.sin_addr, name, max);
    if (addr->family == ABTK_IPV6)
        return (char *)inet_ntop(addr->family, &addr->addr6.sin6_addr, name, max);

    return NULL;
}

int abtk_ifname_fetch(abtk_ifaddrs_t *addrs, int max, int ex_loopback)
{
    struct ifaddrs *results = NULL;
    struct ifaddrs *it = NULL;
    abtk_ifaddrs_t *p = NULL;
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

        if (it->ifa_addr->sa_family != ABTK_IPV4 && it->ifa_addr->sa_family != ABTK_IPV6)
            continue;

        if ((it->ifa_flags & IFF_LOOPBACK) && ex_loopback)
            continue;

        p = &addrs[count++];

        strncpy(p->name, it->ifa_name, IFNAMSIZ);

        if (ABTK_IPV4 == it->ifa_addr->sa_family)
        {
            memcpy(&p->addr, it->ifa_addr, sizeof(struct sockaddr_in));
            memcpy(&p->mark, it->ifa_netmask, sizeof(struct sockaddr_in));

            if (it->ifa_flags & IFF_BROADCAST)
                memcpy(&p->broa, it->ifa_broadaddr, sizeof(struct sockaddr_in));
            else
                p->broa.family = PF_UNSPEC;
        }
        else if (ABTK_IPV6 == it->ifa_addr->sa_family)
        {
            memcpy(&p->addr, it->ifa_addr, sizeof(struct sockaddr_in6));
            memcpy(&p->mark, it->ifa_netmask, sizeof(struct sockaddr_in6));

            /*IPv6 not support. */
            p->broa.family = PF_UNSPEC;
        }
    }

    freeifaddrs(results);

    return count;
}

int abtk_socket_ioctl(uint32_t cmd, void *args)
{
    int sock = -1;
    int chk;

    assert(cmd != 0 && args != NULL);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        return -1;

    chk = ioctl(sock, cmd, args);

final:

    abtk_closep(&sock);

    return chk;
}

char *abtk_mac_fetch(const char *ifname, char addr[12])
{
    struct ifreq args = {0};
    int chk;

    assert(ifname != NULL && addr != NULL);

    strncpy(args.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);

    chk = abtk_socket_ioctl(SIOCGIFHWADDR, &args);
    if (chk == -1)
        return NULL;

    for (int i = 0; i < 6; i++)
        sprintf(addr + 2 * i, "%02X", (uint8_t)args.ifr_hwaddr.sa_data[i]);

    return addr;
}

int abtk_socket_option(int fd,int level, int name,void *data,int *len,int direction)
{
    assert(fd >= 0 && level >= 0 && name > 0 && data != NULL && len != NULL && (direction == 1 || direction == 2));

    if(direction == 1)
        return getsockopt(fd,level,name,data,len);
    
    return setsockopt(fd,level,name,data,*len);
}

int abtk_sockopt_option_int(int fd,int level, int name,int *flag,int direction)
{
    socklen_t len = sizeof(int);

    assert(fd >= 0 && level >= 0 && name > 0 && flag != NULL && (direction == 1 || direction == 2));

    return abtk_socket_option(fd,level,name,flag,&len,direction);
}

int abtk_sockopt_option_timeout(int fd,int name, struct timeval *tv,int direction)
{
    socklen_t len = sizeof(struct timeval);

    assert(fd >= 0 && name > 0 && tv != NULL && (direction == 1 || direction == 2));

    return abtk_socket_option(fd,SOL_SOCKET,name,tv,&len,direction);
}

int abtk_socket_option_linger(int fd,struct linger *lg,int direction)
{
    socklen_t len = sizeof(struct linger);

    assert(fd >= 0 && lg != NULL && (direction == 1 || direction == 2));

    return abtk_socket_option(fd,SOL_SOCKET,SO_LINGER,lg,&len,direction);  
}

int abtk_socket_option_multicast(int fd,abtk_sockaddr_t *multiaddr, const char *ifaddr,int enable)
{
    socklen_t len = sizeof(struct ip_mreq);
    socklen_t len6 = sizeof(struct ipv6_mreq);
    struct ip_mreq st_mreq = {0};
    struct ipv6_mreq st_mreq6 = {0};
    int name;
    int chk;

    assert(fd >= 0 && multiaddr != NULL);

    assert(multiaddr->family == ABTK_IPV4 || multiaddr->family == ABTK_IPV6);

    if(multiaddr->family == ABTK_IPV4)
    {
        st_mreq.imr_multiaddr = multiaddr->addr4.sin_addr;
        st_mreq.imr_interface.s_addr = (ifaddr ? inet_addr(ifaddr) : INADDR_ANY);

        name = (enable ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP);

        chk = abtk_socket_option(fd,IPPROTO_IP,name,&st_mreq,&len,2);  
    }
    else if(multiaddr->family == ABTK_IPV6)
    {
        st_mreq6.ipv6mr_multiaddr = multiaddr->addr6.sin6_addr;
        st_mreq6.ipv6mr_interface = (ifaddr ? if_nametoindex(ifaddr) : 0);

        name = (enable ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);

        chk = abtk_socket_option(fd,IPPROTO_IP,name,&st_mreq,&len,2); 
    }

    return chk;
}

int abtk_socket(sa_family_t family, int udp)
{
    int type = SOCK_CLOEXEC;

    assert(family == ABTK_IPV4 || family == ABTK_IPV6);

    type |= (udp ? SOCK_DGRAM : SOCK_STREAM);

    return socket(family, type, 0);
}

int abtk_bind(int fd, const abtk_sockaddr_t *addr)
{
    assert(fd >= 0 && addr != NULL);

    return bind(fd, &addr->addr, sizeof(abtk_sockaddr_t));
}

int abtk_accept(int fd, abtk_sockaddr_t *addr)
{
    int sub_fd = -1;
    socklen_t addrlen = sizeof(abtk_sockaddr_t);

    assert(fd >= 0);

    if (addr)
        sub_fd = accept(fd, &addr->addr, &addrlen);
    else
        sub_fd = accept(fd, NULL, NULL);

    if (sub_fd < 0)
        return -1;

    /* 添加个非必要标志，忽略可能的出错信息。 */
    abtk_fflag_add(sub_fd, O_CLOEXEC);

    return sub_fd;
}

int abtk_connect(int fd, abtk_sockaddr_t *addr, time_t timeout)
{
    int flags = 0;
    int eno = 0;
    int chk;

    assert(fd >= 0 && addr != NULL);

    flags = abtk_fflag_get(fd);
    if (flags == -1)
        return -1;

    /* 添加非阻塞标志，用于异步连接。*/
    chk = 0;
    if (!(flags & O_NONBLOCK))
        chk = abtk_fflag_add(fd, O_NONBLOCK);

    if (chk != 0)
        return -1;

    chk = connect(fd, &addr->addr, sizeof(abtk_sockaddr_t));
    if(chk == 0)
        goto final;

    if (errno != EINPROGRESS && errno != EWOULDBLOCK && errno != EAGAIN)
        goto final;

    /* 等待写事件(允许)。 */
    chk = (abtk_poll(fd, 0x02, timeout) > 0 ? 0 : -1);
    if(chk != 0)
        goto final;

    /* 获取SOCKET句柄的出错码。 */
    chk = abtk_sockopt_option_int(fd, SOL_SOCKET, SO_ERROR, &eno, 1);
    chk = (eno == 0 ? 0 : -1);

final:
    
    /* 恢复原有的标志，忽略可能的出错信息。*/
    if (!(flags & O_NONBLOCK))
        abtk_fflag_del(fd, O_NONBLOCK);

    return chk;
}

int abtk_sockaddr_from_string(abtk_sockaddr_t *dst, const char *src, int try_lookup)
{
    char name[68] = {0};
    uint16_t port = 0;
    int chk;

    assert(dst != NULL && src != NULL);

    if (strchr(src, '['))
    {
        dst->family = ABTK_IPV6;
        sscanf(src, "%*[[ ]%[^] ]%*[] :,]%hu", name, &port);
    }
    else if (strchr(src, ','))
    {
        dst->family = ABTK_IPV6;
        sscanf(src, "%[^, ]%*[, ]%hu", name, &port);
    }
    else if (strchr(src, ':'))
    {
        dst->family = ABTK_IPV4;
        sscanf(src, "%[^: ]%*[: ]%hu", name, &port);
    }
    else
    {
        strncpy(name,src,62);
    }

    /*尝试直接转换。*/
    chk = abtk_inet_pton(name, dst->family, dst);
    if (chk != 0 && try_lookup)
    {
        /*可能是域名。*/
        chk = (abtk_gethostbyname(name, dst->family, dst, 1, NULL) == 1 ? 0 : -1);
    }

    /*地址转换成功后，再转换端口号。*/
    if(chk==0)
    {
        if(dst->family == ABTK_IPV6)
            dst->addr6.sin6_port = abtk_endian_hton16(port);
        if(dst->family == ABTK_IPV4)
            dst->addr4.sin_port = abtk_endian_hton16(port);
    }

    return chk;
}

char *abtk_sockaddr_to_string(char dst[68],const abtk_sockaddr_t *src)
{
    char buf[INET6_ADDRSTRLEN] = {0};

    assert(dst != NULL && src != NULL);

    assert(src->family == ABTK_IPV4 || src->family == ABTK_IPV6);

    if (abtk_inet_ntop(src, buf, INET6_ADDRSTRLEN) == NULL)
        return NULL;

    if (src->family == ABTK_IPV6)
    {
        if(src->addr6.sin6_port)
            sprintf(dst,"[%s]:%hu",buf,abtk_endian_ntoh16(src->addr6.sin6_port));
        else
            strcpy(dst,buf);
    }
    if (src->family == ABTK_IPV4)
    {
        if(src->addr4.sin_port)
            sprintf(dst,"%s:%hu",buf,abtk_endian_ntoh16(src->addr4.sin_port));
        else
            strcpy(dst,buf);
    }

    return dst;
}

int abtk_sockaddr_where(const abtk_sockaddr_t *test,int where)
{
    int addr_num = 0;
    int addr_max = 100;
    abtk_ifaddrs_t *addrs = NULL;
    abtk_ifaddrs_t *addr_p = NULL;
    int match_num = 0;
    int chk;

    assert(test != NULL && (where ==1 || where ==2));

    addrs = (abtk_ifaddrs_t *)abtk_heap_alloc(sizeof(abtk_ifaddrs_t)*addr_max);
    if(!addrs)
        ABTK_ERRNO_AND_RETURN1(errno,0);

    addr_num = abtk_ifname_fetch(addrs,addr_max,1);

    for (int i = 0; i < addr_num; i++)
    {
        addr_p = addrs+i;// &addrs[i]

        /*只比较同类型的地址。*/
        if(addr_p->addr.family != test->family)
            continue;

        if (addr_p->addr.family == ABTK_IPV6)
            chk = memcmp(&addr_p->addr.addr6.sin6_addr, &test->addr6.sin6_addr, sizeof(struct in6_addr));
        if (addr_p->addr.family == ABTK_IPV4)
            chk = memcmp(&addr_p->addr.addr4.sin_addr, &test->addr4.sin_addr, sizeof(struct in_addr));

        /*地址相同则计数。*/
        if (chk == 0)
            match_num += 1;
    }

    abtk_heap_freep((void**)&addrs);

    if (where == 1)
        return ((match_num > 0) ? 1 : 0);
    if (where == 2)
        return ((match_num <= 0) ? 1 : 0);
}