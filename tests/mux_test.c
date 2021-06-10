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
#include "goodutil/general.h"
#include "goodutil/getargs.h"
#include "goodutil/clock.h"
#include "goodswitch/mux.h"

void test_server(good_tree_t *t)
{
    good_clock_reset();

    good_mux_t *m = good_mux_alloc();
#if 0

    printf("attach begin:%lu\n",good_clock_dot(NULL));

    for (int i = 0; i < 100000; i++)
        assert(good_mux_attach(m, i, 100) == 0);
    //   assert(good_mux_attach(m,10000,100)==0);

    printf("attach cast:%lu\n",good_clock_step(NULL));

    getchar();
    
    printf("attach begin:%lu\n",good_clock_dot(NULL));

    for (int i = 0; i < 100000; i++)
        assert(good_mux_detach(m, i) == 0);

    printf("detach cast:%lu\n",good_clock_step(NULL));
#else

    int l = good_socket(GOOD_IPV4,0);
    good_sockaddr_t a={0};
    good_sockaddr_from_string(&a,"localhost:12345",1);
    bind(l,&a.addr,sizeof(a));
    listen(l,SOMAXCONN);

    assert(good_mux_attach(m, l, 0) == 0);
    assert(good_mux_mark(m,l,GOOD_EPOLL_INPUT,0)==0);

    while(1)
    {
        good_epoll_event e;
        int chk = good_mux_wait(m, &e, 10000);
        if (chk != 0)
            break;

        if(l == e.data.fd)
        {
            int c = accept(l,NULL,0);
            
        }
        else
        {

        }
        
    }

#endif

    good_mux_free(&m);
}

void test_client(good_tree_t *t)
{
    
}

int main(int argc, char **argv)
{
    good_tree_t *t = good_tree_alloc(NULL);

    good_getargs(t,argc,argv,"--");

    if(good_option_exist(t,"--server"))
        test_server(t);
    else 
        test_client(t);

    good_tree_free(&t);

    return 0;
}