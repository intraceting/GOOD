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


int main(int argc, char **argv)
{
    test_gethostbyname();

    return 0;
}