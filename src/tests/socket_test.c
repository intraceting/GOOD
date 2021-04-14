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
    good_ifaddrs_t * addrs = good_heap_alloc(sizeof(good_ifaddrs_t)*10);

    int n = good_ifname_fetch(addrs,10,0);

    for (int i = 0; i < n; i++)
    {
        char addr[100];
        char mark[100];
        char broa[100];
        char mac[100];

        assert(good_inet_ntop(&addrs[i].addr, addr, 100) == addr);
        assert(good_inet_ntop(&addrs[i].mark, mark, 100));

        if(addrs[i].broa.family == GOOD_IPV4 || addrs[i].broa.family == GOOD_IPV6)
            assert(good_inet_ntop(&addrs[i].broa, broa, 100));

        printf("Name: %s\n",addrs[i].name);
        printf("Addr: %s\n",addr);
        printf("Mark: %s\n",mark);
        printf("Broa: %s\n",broa);

        assert(good_mac_fetch(addrs[i].name,mac)==mac);

        printf("MAC: %s\n",mac);

    }

    good_heap_free(addrs);
}

void test_connect()
{
    good_sockaddr_t addr={0};

    //int n = good_gethostbyname("www.taobao.com",GOOD_IPV4,&addr,1,NULL);
    //int n = good_gethostbyname("localhost",GOOD_IPV4,&addr,1,NULL);
    good_inet_pton("192.168.100.4",GOOD_IPV4,&addr);
    addr.addr4.sin_port = good_endian_hton16(8090);

    int s = good_socket(GOOD_IPV4,0);

  //  good_fflag_add(s,O_NONBLOCK);
    
    assert(good_connect(s,&addr,10000)==0);
    



    good_closep(&s);
}


int main(int argc, char **argv)
{
    //test_gethostbyname();

    //test_ifname();

    test_connect();

    return 0;
}