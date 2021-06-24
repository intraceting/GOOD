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
#include <sys/sendfile.h>
#include "abtkutil/general.h"
#include "abtkutil/getargs.h"
#include "abtkutil/clock.h"
#include "abtkutil/thread.h"
#include "abtkutil/signal.h"
#include "abtkcomm/mux.h"

void* sigwaitinfo_cb(void* args)
{
    abtk_signal_t sig;
    sigfillset(&sig.signals);

    sigdelset(&sig.signals, SIGKILL);
    sigdelset(&sig.signals, SIGSEGV);
    sigdelset(&sig.signals, SIGSTOP);

    sig.signal_cb = NULL;
    sig.opaque = NULL;
     
    abtk_sigwaitinfo(&sig,-1);

    return NULL;
}

void* server_loop(void* args)
{
    abtk_mux_t *m = (abtk_mux_t *)args;
    static volatile pthread_t leader = 0;
    static int l = -1;

    if (abtk_thread_leader_test(&leader) == 0)
    {

        l = abtk_socket(ABTK_IPV4, 0);
        abtk_sockaddr_t a = {0};
        abtk_sockaddr_from_string(&a, "localhost:12345", 1);

        int flag = 1;
        abtk_sockopt_option_int(l, SOL_SOCKET, SO_REUSEPORT, &flag, 2);
        abtk_sockopt_option_int(l, SOL_SOCKET, SO_REUSEADDR, &flag, 2);

        abtk_bind(l, &a);
        listen(l, SOMAXCONN);

        assert(abtk_mux_attach(m, l, 0) == 0);
        assert(abtk_mux_mark(m, l, ABTK_EPOLL_INPUT, 0) == 0);
    }

    while(1)
    {
        abtk_epoll_event e;
        int chk = abtk_mux_wait(m, &e, -1);
        if (chk < 0)
            break;

        if(e.events & ABTK_EPOLL_ERROR)
        {
            assert(abtk_mux_mark(m,e.data.fd,0,e.events)==0);
            assert(abtk_mux_unref(m,&e)==0);
            assert(abtk_mux_detach(m,e.data.fd)==0);
            abtk_closep(&e.data.fd);
        }
        else if(e.data.fd == l)
        {
            while(1)
            {
                int c = abtk_accept(e.data.fd,NULL);
                if(c<0)
                    break;

                int flag=1;
                assert(abtk_sockopt_option_int(c, IPPROTO_TCP, TCP_NODELAY,&flag, 2) == 0);

                assert(abtk_mux_attach(m, c,10*1000) == 0);
                assert(abtk_mux_mark(m,c,ABTK_EPOLL_INPUT,0)==0);
            }
            
            assert(abtk_mux_mark(m,e.data.fd,ABTK_EPOLL_INPUT,ABTK_EPOLL_INPUT)==0);
        }
        else
        {
            if(e.events & ABTK_EPOLL_INPUT)
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
                        assert(abtk_mux_mark(m,e.data.fd, ABTK_EPOLL_INPUT|ABTK_EPOLL_OUTPUT, ABTK_EPOLL_INPUT) == 0);

                    break;
                }
            }
            if(e.events & ABTK_EPOLL_OUTPUT)
            {
                while (1)
                {
                    static int fd = -1;
                    if(fd ==-1)
                        fd = abtk_open("/tmp/mysql-trace.log",0,0,0);
#if 0 
                    char buf[100];
                    size_t n = abtk_read(fd, buf, 100);
                    if (n <= 0)
                    {
                        abtk_closep(&fd);
                        assert(abtk_mux_mark(m,e.data.fd, 0, ABTK_EPOLL_OUTPUT) == 0);
                        break;
                    }

                    //memset(buf,rand()%26+40,100);
                    int s = send(e.data.fd, buf, 100, 0);
                    if (s > 0)
                        continue;
                    if (s == -1 && errno == EAGAIN)
                        assert(abtk_mux_mark(m,e.data.fd, ABTK_EPOLL_OUTPUT, ABTK_EPOLL_OUTPUT) == 0);
#else
                    ssize_t s = sendfile(e.data.fd,fd,NULL,100000000);
                    printf("s=%ld\n",s);
                    if(s>0)
                        continue;
                    if (s == 0)
                    {
                        abtk_closep(&fd);
                        assert(abtk_mux_mark(m,e.data.fd, 0, ABTK_EPOLL_OUTPUT) == 0);
                        break;
                    }
                    else if (s == -1 && errno == EAGAIN)
                    {
                        assert(abtk_mux_mark(m,e.data.fd, ABTK_EPOLL_OUTPUT, ABTK_EPOLL_OUTPUT) == 0);
                    }
#endif
                    break;
                }
            }

            assert(abtk_mux_unref(m,&e)==0);
        }
        
    }
}

void test_server(abtk_tree_t *t)
{
    abtk_clock_reset();

    abtk_mux_t *m = abtk_mux_alloc();
#if 0

    printf("attach begin:%lu\n",abtk_clock_dot(NULL));

    for (int i = 0; i < 100000; i++)
        assert(abtk_mux_attach(m, i, 100) == 0);
    //   assert(abtk_mux_attach(m,10000,100)==0);

    printf("attach cast:%lu\n",abtk_clock_step(NULL));

    getchar();
    
    printf("attach begin:%lu\n",abtk_clock_dot(NULL));

    for (int i = 0; i < 100000; i++)
        assert(abtk_mux_detach(m, i) == 0);

    printf("detach cast:%lu\n",abtk_clock_step(NULL));
#else

    #pragma omp parallel for num_threads(3)
    for (int i = 0; i < 3; i++)
    {
#if 0
        abtk_thread_t p;
        p.routine = server_loop;
        p.opaque = m;
        abtk_thread_create(&p, 0);
#else 
        server_loop(m);
#endif 
    }

    while (getchar()!='Q');    

#endif

    abtk_mux_free(&m);
}

void test_client(abtk_tree_t *t)
{
    int c = abtk_socket(ABTK_IPV4, 0);
    abtk_sockaddr_t a = {0};
    abtk_sockaddr_from_string(&a, abtk_option_get(t,"--",1,"localhost:12345"), 1);

    int chk = abtk_connect(c,&a,10000);
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

    abtk_closep(&c);
}

int main(int argc, char **argv)
{
    abtk_tree_t *t = abtk_tree_alloc(NULL);

    abtk_getargs(t,argc,argv,"--");

    abtk_thread_t p;
    p.routine = sigwaitinfo_cb;
    abtk_thread_create(&p,0);


    if(abtk_option_exist(t,"--server"))
        test_server(t);
    else 
        test_client(t);

    abtk_tree_free(&t);

    return 0;
}