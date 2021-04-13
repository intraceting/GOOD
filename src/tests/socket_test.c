/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "libutil/socket.h"

void test_gethostbyname()
{
    good_sockaddr_t * addrs = good_heap_alloc(sizeof(good_sockaddr_t)*10);

    char*canonname = NULL;
    //int n = good_gethostbyname("ip6-localhost",GOOD_IPV6,addr,10);
    int n = good_gethostbyname("www.taobao.com",GOOD_IPV4,addrs,10,&canonname);
    
    printf("%s\n",canonname);

    for (int i = 0; i < n; i++)
    {
        char buf[100];

        assert(good_inet_ntop(&addrs[i], buf, 100));

        printf("%s\n",buf);
    }

    good_heap_free(addrs);
}

void test_ifname()
{
    good_ifname_t * names = good_heap_alloc(sizeof(good_ifname_t)*10);

    int n = good_ifname_fetch(names,10,0);

    for (int i = 0; i < n; i++)
    {
        char addr[100];
        char mark[100];
        char broa[100];

        assert(good_inet_ntop(&names[i].addr, addr, 100) == addr);
        assert(good_inet_ntop(&names[i].mark, mark, 100));

        if(names[i].broa.family == GOOD_IPV4 || names[i].broa.family == GOOD_IPV6)
            assert(good_inet_ntop(&names[i].broa, broa, 100));

        printf("Name: %s\n",names[i].name);
        printf("Addr: %s\n",addr);
        printf("Mark: %s\n",mark);
        printf("Broa: %s\n",broa);
    }

    good_heap_free(names);
}


int main(int argc, char **argv)
{
    //test_gethostbyname();

    test_ifname();

    return 0;
}