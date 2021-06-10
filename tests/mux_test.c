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
#include "goodutil/thread.h"
#include "goodutil/signal.h"
#include "goodswitch/mux.h"

void* sigwaitinfo_cb(void* args)
{
    good_signal_t sig;
    sigfillset(&sig.signals);

    sigdelset(&sig.signals, SIGKILL);
    sigdelset(&sig.signals, SIGSEGV);
    sigdelset(&sig.signals, SIGSTOP);

    sig.signal_cb = NULL;
    sig.opaque = NULL;
     
    good_sigwaitinfo(&sig,-1);

    return NULL;
}

void* server_loop(void* args)
{
    good_mux_t *m = (good_mux_t *)args;
    static volatile pthread_t leader = 0;
    static int l = -1;

    if (good_thread_leader_test(&leader) == 0)
    {

        l = good_socket(GOOD_IPV4, 0);
        good_sockaddr_t a = {0};
        good_sockaddr_from_string(&a, "localhost:12345", 1);

        int flag = 1;
        good_sockopt_option_int(l, SOL_SOCKET, SO_REUSEPORT, &flag, 2);
        good_sockopt_option_int(l, SOL_SOCKET, SO_REUSEADDR, &flag, 2);

        bind(l, &a.addr, sizeof(a));
        listen(l, SOMAXCONN);

        assert(good_mux_attach(m, l, 0) == 0);
        assert(good_mux_mark(m, l, GOOD_EPOLL_INPUT, 0) == 0);
    }

    while(1)
    {
        good_epoll_event e;
        int chk = good_mux_wait(m, &e, 10000);
        if (chk < 0)
            break;

        if(e.events & GOOD_EPOLL_ERROR)
        {
            assert(good_mux_mark(m,e.data.fd,0,e.events)==0);
            assert(good_mux_detach(m,e.data.fd)==0);
            close(e.data.fd);
        }
        else if(e.data.fd == l)
        {
            while(1)
            {
                int c = accept(e.data.fd,NULL,0);
                if(c<0)
                    break;

                int flag=1;
                good_sockopt_option_int(c, IPPROTO_TCP, TCP_NODELAY,&flag, 2);

                assert(good_mux_attach(m, c,10*1000) == 0);
                assert(good_mux_mark(m,c,GOOD_EPOLL_INPUT,0)==0);
            }
            
            assert(good_mux_mark(m,e.data.fd,GOOD_EPOLL_INPUT,0)==0);
        }
        else
        {
            if(e.events & GOOD_EPOLL_INPUT)
            {
                while (1)
                {
                    char buf[100];
                    int r = recv(e.data.fd, buf, 100, 0);
                    if (r > 0)
                    {
                        printf("%s\n",buf);
                        continue;
                    }
                    if (r == -1 && errno == EAGAIN)
                        assert(good_mux_mark(m,e.data.fd, GOOD_EPOLL_INPUT|GOOD_EPOLL_OUTPUT, 0) == 0);

                    break;
                }
            }
            if(e.events & GOOD_EPOLL_OUTPUT)
            {
                while (1)
                {
                    static int fd = -1;
                    if(fd ==-1)
                        fd = good_open("/tmp/mysql-trace.log",0,0,0);

                    char buf[100];
                    size_t n = good_read(fd, buf, 100);
                    if (n <= 0)
                    {
                        good_closep(&fd);
                        break;
                    }

                    //memset(buf,rand()%26+40,100);
                    int s = send(e.data.fd, buf, 100, 0);
                    if (s > 0)
                        continue;
                    if (s == -1 && errno == EAGAIN)
                        assert(good_mux_mark(m,e.data.fd, GOOD_EPOLL_OUTPUT, 0) == 0);

                    break;
                }
            }

            assert(good_mux_mark(m,e.data.fd,0,e.events)==0);
        }
        
    }
}

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

    for (int i = 0; i < 3; i++)
    {
        good_thread_t p;
        p.routine = server_loop;
        p.opaque = m;
        good_thread_create(&p, 0);
    }

    while (getchar()!='Q');    

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

    good_thread_t p;
    p.routine = sigwaitinfo_cb;
    good_thread_create(&p,0);


    if(good_option_exist(t,"--server"))
        test_server(t);
    else 
        test_client(t);

    good_tree_free(&t);

    return 0;
}