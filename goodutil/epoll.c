/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "epoll.h"

int good_epoll_create()
{
    int fd = epoll_create(1024);
    if (fd < 0)
        return -1;

    /* 添加个非必要标志，忽略可能的出错信息。 */
    good_fflag_add(fd, SOCK_CLOEXEC);

    return fd;
}

int good_epoll_mark(int efd, int fd, const good_epoll_event *event, int first)
{
    int opt = 0;
    good_epoll_event mark = {0,0};

    assert(efd >= 0 && fd >= 0 && event != NULL);
    assert(event->events & (GOOD_EPOLL_INPUT | GOOD_EPOLL_OUTPUT));

    mark.data = event->data;
    mark.events |= (EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET);

    /*转换事件。*/
    if (event->events & GOOD_EPOLL_INPUT)
        mark.events |= EPOLLIN;
    if (event->events & GOOD_EPOLL_OUTPUT)
        mark.events |= EPOLLOUT;

    opt = (first ? EPOLL_CTL_ADD : EPOLL_CTL_MOD);

    return epoll_ctl(efd, opt, fd, &mark);
}

int good_epoll_drop(int efd, int fd)
{
    assert(efd >= 0 && fd >= 0);

    return epoll_ctl(efd,EPOLL_CTL_DEL,fd, NULL);
}

int good_epoll_wait(int efd,good_epoll_event* events,int max,int timeout)
{
    int chk;
    uint32_t tmp;
    
    assert(efd >= 0 && events != NULL && max > 0);

    chk = epoll_wait(efd,events,max,timeout);

    /*转换事件。 */
    for (int i = 0; i < chk; i++)
    {
        tmp = events[i].events;
        events[i].events = 0;

        if(tmp & EPOLLIN)
            events[i].events |= GOOD_EPOLL_INPUT;
        if(tmp & EPOLLPRI)
            events[i].events |= GOOD_EPOLL_INOOB;
        if(tmp & EPOLLOUT)
            events[i].events |= GOOD_EPOLL_OUTPUT;
        if(tmp & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            events[i].events |= GOOD_EPOLL_ERROR;
    }

    return chk;
}