/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_EPOLL_H
#define GOOD_UTIL_EPOLL_H

#include "general.h"

/**
 * EOPLL的事件。
*/
enum _good_epoll_event
{
    /**
     * 输入事件。
    */
    GOOD_EPOLL_INPUT = 0x0001,
#define GOOD_EPOLL_INPUT GOOD_EPOLL_INPUT

    /**
     * 带外(OOB)输入事件。
    */
    GOOD_EPOLL_INOOB = 0x0002,
#define GOOD_EPOLL_INOOB GOOD_EPOLL_INOOB

    /**
     * 输出事件。
    */
    GOOD_EPOLL_OUTPUT = 0x0010,
#define GOOD_EPOLL_OUTPUT GOOD_EPOLL_OUTPUT

    /**
     * 出错事件。
    */
    GOOD_EPOLL_ERROR = 0x1000
#define GOOD_EPOLL_ERROR GOOD_EPOLL_ERROR
};

/**
 * 创建EPOLL句柄
 * 
 * @return >=0 成功(EPOLL句柄)，-1 失败。
*/
int good_epoll_create();

/**
 * 注册句柄(文件或SOCKET)和事件。
 * 
 * @param first 是否首次注册。0 否，!0 是。
 * 
 * @return 0 成功，-1 失败。
*/
int good_epoll_mark(int efd, int fd,const struct epoll_event * event, int first);

/**
 * 删除句柄(文件或SOCKET)和事件。
 * 
 * @return 0 成功，-1 失败。
*/
int good_epoll_drop(int efd, int fd);

/**
 * 等待事件。
 * 
 * @param timeout 超时(毫秒)。>= 0 有事件或时间过期，< 0 直到有事件或出错。
 * 
 * @return > 0 事件数量，<= 0 超时或出错。
*/
int good_epoll_wait(int efd,struct epoll_event * events,int max,int timeout);

#endif //GOOD_UTIL_EPOLL_H