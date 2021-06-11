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
#include <sys/sendfile.h>
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

        good_bind(l, &a);
        listen(l, SOMAXCONN);

        assert(good_mux_attach(m, l, 0) == 0);
        assert(good_mux_mark(m, l, GOOD_EPOLL_INPUT, 0) == 0);
    }

    while(1)
    {
        good_epoll_event e;
        int chk = good_mux_wait(m, &e, -1);
        if (chk < 0)
            break;

        if(e.events & GOOD_EPOLL_ERROR)
        {
            assert(good_mux_mark(m,e.data.fd,0,e.events)==0);
            assert(good_mux_unref(m,&e)==0);
            assert(good_mux_detach(m,e.data.fd)==0);
            good_closep(&e.data.fd);
        }
        else if(e.data.fd == l)
        {
            while(1)
            {
                int c = good_accept(e.data.fd,NULL);
                if(c<0)
                    break;

                int flag=1;
                assert(good_sockopt_option_int(c, IPPROTO_TCP, TCP_NODELAY,&flag, 2) == 0);

                assert(good_mux_attach(m, c,10*1000) == 0);
                assert(good_mux_mark(m,c,GOOD_EPOLL_INPUT,0)==0);
            }
            
            assert(good_mux_mark(m,e.data.fd,GOOD_EPOLL_INPUT,GOOD_EPOLL_INPUT)==0);
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
                        assert(good_mux_mark(m,e.data.fd, GOOD_EPOLL_INPUT|GOOD_EPOLL_OUTPUT, GOOD_EPOLL_INPUT) == 0);

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
#if 0 
                    char buf[100];
                    size_t n = good_read(fd, buf, 100);
                    if (n <= 0)
                    {
                        good_closep(&fd);
                        assert(good_mux_mark(m,e.data.fd, 0, GOOD_EPOLL_OUTPUT) == 0);
                        break;
                    }

                    //memset(buf,rand()%26+40,100);
                    int s = send(e.data.fd, buf, 100, 0);
                    if (s > 0)
                        continue;
                    if (s == -1 && errno == EAGAIN)
                        assert(good_mux_mark(m,e.data.fd, GOOD_EPOLL_OUTPUT, GOOD_EPOLL_OUTPUT) == 0);
#else
                    ssize_t s = sendfile(e.data.fd,fd,NULL,100000000);
                    printf("s=%ld\n",s);
                    if(s>0)
                        continue;
                    if (s == 0)
                    {
                        good_closep(&fd);
                        assert(good_mux_mark(m,e.data.fd, 0, GOOD_EPOLL_OUTPUT) == 0);
                        break;
                    }
                    else if (s == -1 && errno == EAGAIN)
                    {
                        assert(good_mux_mark(m,e.data.fd, GOOD_EPOLL_OUTPUT, GOOD_EPOLL_OUTPUT) == 0);
                    }
#endif
                    break;
                }
            }

            assert(good_mux_unref(m,&e)==0);
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

    #pragma omp parallel for num_threads(3)
    for (int i = 0; i < 3; i++)
    {
#if 0
        good_thread_t p;
        p.routine = server_loop;
        p.opaque = m;
        good_thread_create(&p, 0);
#else 
        server_loop(m);
#endif 
    }

    while (getchar()!='Q');    

#endif

    good_mux_free(&m);
}

void test_client(good_tree_t *t)
{
    int c = good_socket(GOOD_IPV4, 0);
    good_sockaddr_t a = {0};
    good_sockaddr_from_string(&a, good_option_get(t,"--",1,"localhost:12345"), 1);

    int chk = good_connect(c,&a,10000);
    assert(chk==0);

    send(c,"aaa\n",3,0);

    while(1)
    {   
        char buf[100] ={0};
        int r = recv(c,buf,100,0);
        if(r<=0)
            break;
        printf("%s\n",buf);
    }

    good_closep(&c);
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