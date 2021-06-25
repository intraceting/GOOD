/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKCOMM_MUX_H
#define ABCDKCOMM_MUX_H

#include "abcdkutil/map.h"
#include "abcdkutil/pool.h"
#include "abcdkutil/thread.h"
#include "abcdkutil/clock.h"
#include "abcdkutil/epoll.h"
#include "abcdkutil/socket.h"

/** 多路复用器。*/
typedef struct _abcdk_mux abcdk_mux_t;

/**
 * 销毁多路复用器环境。
*/
void abcdk_mux_free(abcdk_mux_t **ctx);

/**
 * 创建多路复用器环境。
 * 
 * @return !NULL(0) 成功(环境指针)，NULL(0) 失败。
*/
abcdk_mux_t *abcdk_mux_alloc();

/**
 * 分离句柄。
 * 
 * @warning 关联成功后，句柄在分离前不可被关闭或释放。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int abcdk_mux_detach(abcdk_mux_t *ctx,int fd);

/**
 * 关联句柄。
 * 
 * @warning 关联成功后，句柄在分离前不可被关闭或释放。
 * 
 * @return 0 成功，!0 失败(或重复)。
*/
int abcdk_mux_attach(abcdk_mux_t *ctx,int fd,time_t timeout);

/**
 * 注册事件。
 * 
 * @param fd 句柄，-1 广播到所有句柄。
 * @param want 希望的事件。
 * @param done 完成的事件，广播无效。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int abcdk_mux_mark(abcdk_mux_t *ctx,int fd,uint32_t want,uint32_t done);

/**
 * 等待事件。
 * 
 * @param timeout 超时(毫秒)。
 * 
 * @return >=0 成功，!0 失败(或超时)。
*/
int abcdk_mux_wait(abcdk_mux_t *ctx,abcdk_epoll_event *event,time_t timeout);

/**
 * 引用释放。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int abcdk_mux_unref(abcdk_mux_t *ctx,abcdk_epoll_event *event);


#endif //ABCDKCOMM_MUX_H