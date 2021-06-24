/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "epoll.h"

int abtk_epoll_create()
{
    int fd = epoll_create(1024);
    if (fd < 0)
        return -1;

    /* 添加个非必要标志，忽略可能的出错信息。 */
    abtk_fflag_add(fd, SOCK_CLOEXEC);

    return fd;
}

int abtk_epoll_mark(int efd, int fd, const abtk_epoll_event *event, int first)
{
    int opt = 0;
    abtk_epoll_event mark = {0, 0};

    assert(efd >= 0 && fd >= 0 && event != NULL);
    assert((event->events & ~(ABTK_EPOLL_INPUT | ABTK_EPOLL_INOOB | ABTK_EPOLL_OUTPUT | ABTK_EPOLL_ERROR)) == 0);

    /*如果注册事件中包括错误事件，则直接跳转出错流程。*/
    if (event->events & ABTK_EPOLL_ERROR)
        goto final_error;

    mark.data = event->data;
    mark.events |= (EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET);

    /*转换事件。*/
    if (event->events & ABTK_EPOLL_INPUT)
        mark.events |= EPOLLIN;
    if (event->events & ABTK_EPOLL_INOOB)
        mark.events |= EPOLLPRI;
    if (event->events & ABTK_EPOLL_OUTPUT)
        mark.events |= EPOLLOUT;

    opt = (first ? EPOLL_CTL_ADD : EPOLL_CTL_MOD);

    return epoll_ctl(efd, opt, fd, &mark);

final_error:

    return -1;
}

int abtk_epoll_drop(int efd, int fd)
{
    assert(efd >= 0 && fd >= 0);

    return epoll_ctl(efd,EPOLL_CTL_DEL,fd, NULL);
}

int abtk_epoll_wait(int efd,abtk_epoll_event* events,int max,time_t timeout)
{
    int chk;
    uint32_t tmp;
    
    assert(efd >= 0 && events != NULL && max > 0);

    chk = epoll_wait(efd, events, max, (timeout >= INT32_MAX ? -1 : timeout));

    /*转换事件。 */
    for (int i = 0; i < chk; i++)
    {
        tmp = events[i].events;
        events[i].events = 0;

        if(tmp & EPOLLIN)
            events[i].events |= ABTK_EPOLL_INPUT;
        if(tmp & EPOLLPRI)
            events[i].events |= ABTK_EPOLL_INOOB;
        if(tmp & EPOLLOUT)
            events[i].events |= ABTK_EPOLL_OUTPUT;
        if(tmp & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            events[i].events |= ABTK_EPOLL_ERROR;
    }

    return chk;
}