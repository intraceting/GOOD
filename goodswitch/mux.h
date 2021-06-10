/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODSWITCH_MUX_H
#define GOODSWITCH_MUX_H

#include "goodutil/map.h"
#include "goodutil/pool.h"
#include "goodutil/thread.h"
#include "goodutil/clock.h"
#include "goodutil/epoll.h"
#include "goodutil/socket.h"

/** 多路复用器。*/
typedef struct _good_mux good_mux_t;

/**
 * 销毁多路复用器环境。
*/
void good_mux_free(good_mux_t **ctx);

/**
 * 创建多路复用器环境。
 * 
 * @return !NULL(0) 成功(环境指针)，NULL(0) 失败。
*/
good_mux_t *good_mux_alloc();

/**
 * 分离句柄。
 * 
 * @warning 关联成功后，句柄在分离前不可被关闭或释放。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int good_mux_detach(good_mux_t *ctx,int fd);

/**
 * 关联句柄。
 * 
 * @warning 关联成功后，句柄在分离前不可被关闭或释放。
 * 
 * @return 0 成功，!0 失败(或重复)。
*/
int good_mux_attach(good_mux_t *ctx,int fd,time_t timeout);

/**
 * 注册事件。
 * 
 * @param concerned 关心的事件。
 * @param completed 完成的事件。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int good_mux_mark(good_mux_t *ctx,int fd,uint32_t concerned,uint32_t completed);

/**
 * 等待事件。
 * 
 * @param timeout 超时(毫秒)。
 * 
 * @return >=0 成功，!0 失败(或超时)。
*/
int good_mux_wait(good_mux_t *ctx,good_epoll_event *event,time_t timeout);


#endif //GOODSWITCH_MUX_H