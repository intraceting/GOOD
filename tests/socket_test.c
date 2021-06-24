/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "abtkutil/socket.h"

void test_gethostbyname()
{
    abtk_sockaddr_t * addrs = abtk_heap_alloc(sizeof(abtk_sockaddr_t)*10);

    char*canonname = NULL;
    //int n = abtk_gethostbyname("ip6-localhost",ABTK_IPV6,addr,10);
    int n = abtk_gethostbyname("www.taobao.com",ABTK_IPV4,addrs,10,&canonname);
    
    printf("%s\n",canonname);

    for (int i = 0; i < n; i++)
    {
        char buf[100];

        assert(abtk_inet_ntop(&addrs[i], buf, 100));

        printf("%s\n",buf);
    }

    abtk_heap_free(addrs);
}

void test_ifname()
{
    abtk_ifaddrs_t * addrs = abtk_heap_alloc(sizeof(abtk_ifaddrs_t)*10);

    int n = abtk_ifname_fetch(addrs,10,0);

    for (int i = 0; i < n; i++)
    {
        char addr[100];
        char mark[100];
        char broa[100];
        char mac[100];

        assert(abtk_inet_ntop(&addrs[i].addr, addr, 100) == addr);
        assert(abtk_inet_ntop(&addrs[i].mark, mark, 100));

        if(addrs[i].broa.family == ABTK_IPV4 || addrs[i].broa.family == ABTK_IPV6)
            assert(abtk_inet_ntop(&addrs[i].broa, broa, 100));

        printf("Name: %s\n",addrs[i].name);
        printf("Addr: %s\n",addr);
        printf("Mark: %s\n",mark);
        printf("Broa: %s\n",broa);

        assert(abtk_mac_fetch(addrs[i].name,mac)==mac);

        printf("MAC: %s\n",mac);

    }

    abtk_heap_free(addrs);
}

void test_connect()
{
    abtk_sockaddr_t addr={0};

    //int n = abtk_gethostbyname("www.taobao.com",ABTK_IPV4,&addr,1,NULL);
    //int n = abtk_gethostbyname("localhost",ABTK_IPV4,&addr,1,NULL);
    //abtk_inet_pton("192.168.100.4",ABTK_IPV4,&addr);
    //addr.addr4.sin_port = abtk_endian_hton16(8090);

    assert(abtk_sockaddr_from_string(&addr,"www.taobao.com:443",1)==0);
  //  assert(abtk_sockaddr_from_string(&addr,"192.168.100.4:8090",0)==0);
    //assert(abtk_sockaddr_from_string(&addr,"[www.taobao.com]:443",1)==0);
    //assert(abtk_sockaddr_from_string(&addr,"[240e:cf:9000:1::3fb]:443",1)==0);
   // assert(abtk_sockaddr_from_string(&addr,"[240e:cf:9000:1::3fb],443",1)==0);


    char buf[100]={0};
    printf("%s\n",abtk_sockaddr_to_string(buf,&addr));

    assert(abtk_sockaddr_where(&addr,1)==0);

    assert(abtk_sockaddr_where(&addr,2)!=0);

    int s = abtk_socket(addr.family,0);

  //  abtk_fflag_add(s,O_NONBLOCK);
    
    assert(abtk_connect(s,&addr,10000)==0);
    



    abtk_closep(&s);
}

void test_group()
{
    int s = abtk_socket(ABTK_IPV4,1);

    abtk_sockaddr_t addr={0};
    abtk_inet_pton("224.0.0.5",ABTK_IPV4,&addr);
    addr.addr4.sin_port = abtk_endian_hton16(8090);

    assert(abtk_socket_option_multicast(s,&addr,NULL,1)==0);

    assert(abtk_bind(s,&addr)==0);


    abtk_closep(&s);
}

int main(int argc, char **argv)
{
    //test_gethostbyname();

    //test_ifname();

    test_connect();

  //  test_group();

    return 0;
}