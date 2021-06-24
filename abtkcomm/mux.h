/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKCOMM_MUX_H
#define ABTKCOMM_MUX_H

#include "abtkutil/map.h"
#include "abtkutil/pool.h"
#include "abtkutil/thread.h"
#include "abtkutil/clock.h"
#include "abtkutil/epoll.h"
#include "abtkutil/socket.h"

/** 多路复用器。*/
typedef struct _abtk_mux abtk_mux_t;

/**
 * 销毁多路复用器环境。
*/
void abtk_mux_free(abtk_mux_t **ctx);

/**
 * 创建多路复用器环境。
 * 
 * @return !NULL(0) 成功(环境指针)，NULL(0) 失败。
*/
abtk_mux_t *abtk_mux_alloc();

/**
 * 分离句柄。
 * 
 * @warning 关联成功后，句柄在分离前不可被关闭或释放。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int abtk_mux_detach(abtk_mux_t *ctx,int fd);

/**
 * 关联句柄。
 * 
 * @warning 关联成功后，句柄在分离前不可被关闭或释放。
 * 
 * @return 0 成功，!0 失败(或重复)。
*/
int abtk_mux_attach(abtk_mux_t *ctx,int fd,time_t timeout);

/**
 * 注册事件。
 * 
 * @param fd 句柄，-1 广播到所有句柄。
 * @param want 希望的事件。
 * @param done 完成的事件，广播无效。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int abtk_mux_mark(abtk_mux_t *ctx,int fd,uint32_t want,uint32_t done);

/**
 * 等待事件。
 * 
 * @param timeout 超时(毫秒)。
 * 
 * @return >=0 成功，!0 失败(或超时)。
*/
int abtk_mux_wait(abtk_mux_t *ctx,abtk_epoll_event *event,time_t timeout);

/**
 * 引用释放。
 * 
 * @return 0 成功，!0 失败(或不存在)。
*/
int abtk_mux_unref(abtk_mux_t *ctx,abtk_epoll_event *event);


#endif //ABTKCOMM_MUX_H