/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_EPOLL_H
#define ABTKUTIL_EPOLL_H

#include "general.h"

/**
 * EOPLL的事件。
*/
enum _abtk_epoll_event
{
    /**
     * 输入事件。
    */
    ABTK_EPOLL_INPUT = 0x00000001,
#define ABTK_EPOLL_INPUT ABTK_EPOLL_INPUT

    /**
     * 带外(OOB)输入事件。
    */
    ABTK_EPOLL_INOOB = 0x00000002,
#define ABTK_EPOLL_INOOB ABTK_EPOLL_INOOB

    /**
     * 输出事件。
    */
    ABTK_EPOLL_OUTPUT = 0x00000100,
#define ABTK_EPOLL_OUTPUT ABTK_EPOLL_OUTPUT

    /**
     * 出错事件。
    */
    ABTK_EPOLL_ERROR = 0x01000000
#define ABTK_EPOLL_ERROR ABTK_EPOLL_ERROR
};

/**
 * 事件结构体。
 * 
 * @warning 不能使用原始事件的值。
*/
typedef struct epoll_event abtk_epoll_event;

/**
 * 创建EPOLL句柄
 * 
 * @return >=0 成功(EPOLL句柄)，-1 失败。
*/
int abtk_epoll_create();

/**
 * 注册句柄(文件或SOCKET)和事件。
 * 
 * @param first 是否首次注册。0 否，!0 是。
 * 
 * @return 0 成功，-1 失败。
*/
int abtk_epoll_mark(int efd, int fd,const abtk_epoll_event *event, int first);

/**
 * 删除句柄(文件或SOCKET)和事件。
 * 
 * @return 0 成功，-1 失败。
*/
int abtk_epoll_drop(int efd, int fd);

/**
 * 等待事件。
 * 
 * @param timeout 超时(毫秒)。>= 0 有事件或时间过期，< 0 直到有事件或出错。
 * 
 * @return > 0 事件数量，<= 0 超时或出错。
*/
int abtk_epoll_wait(int efd,abtk_epoll_event *events,int max,time_t timeout);

#endif //ABTKUTIL_EPOLL_H