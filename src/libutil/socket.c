/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "socket.h"

ssize_t good_gethostbyname(const char *name, sa_family_t family, good_sockaddr_t *addrs, size_t max,char** canonname)
{
    struct addrinfo *results = NULL;
    struct addrinfo *it = NULL;
    struct addrinfo hint = {0};
    int chk;
    ssize_t count = 0;

    assert(name != NULL && (family == GOOD_IPV4 || family == GOOD_IPV6) && addrs != NULL && max > 0);

    hint.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
    hint.ai_family = family;

    chk = getaddrinfo(name, NULL, &hint, &results);
    if(chk != 0 || results == NULL)
        return -1;

    for (it = results; it != NULL && count < max ; it = it->ai_next)
    {
        if (it->ai_socktype != SOCK_STREAM || it->ai_protocol != IPPROTO_TCP)
            continue;

        if (it->ai_addr->sa_family != GOOD_IPV4 && it->ai_addr->sa_family != GOOD_IPV6)
            continue;

        memcpy(&addrs[count++], it->ai_addr, it->ai_addrlen);
    }

    if(canonname && results->ai_canonname)
        *canonname = good_heap_clone(results->ai_canonname,strlen(results->ai_canonname)+1);

    freeaddrinfo(results);

    return count;
}

int good_inet_pton(const char* name,sa_family_t family,good_sockaddr_t *addr)
{
    assert(name != NULL && (family == GOOD_IPV4 || family == GOOD_IPV6) && addr != NULL);

    /*
     * bind family
    */
    addr->family = family;

    if(addr->family == GOOD_IPV4)
        return inet_pton(family, name, &addr->addr4.sin_addr);
    if(addr->family == GOOD_IPV6)
        return inet_pton(family, name, &addr->addr6.sin6_addr);

    return -1;
}

char* good_inet_ntop(good_sockaddr_t *addr,char* name,size_t max)
{
    assert(addr != NULL && name != NULL && max > 0); 
    assert(addr->family == GOOD_IPV4 || addr->family == GOOD_IPV6);
    assert((addr->family == GOOD_IPV4) ? (max >= INET_ADDRSTRLEN) : 1);
    assert((addr->family == GOOD_IPV6) ? (max >= INET6_ADDRSTRLEN) : 1);

    if(addr->family == GOOD_IPV4)
        return inet_ntop(addr->family, &addr->addr4.sin_addr,name, max);
    if(addr->family == GOOD_IPV6)
        return inet_ntop(addr->family, &addr->addr6.sin6_addr,name, max);

    return NULL;
}