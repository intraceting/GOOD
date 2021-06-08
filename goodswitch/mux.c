/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "mux.h"

/**
 * 多路复用器。
*/
typedef struct _good_mux
{
    /** 互斥量。*/
    good_mutex_t mutex;

    /** 节点表。*/
    good_map_t nodes;

    /** 事件池。*/
    good_pool_t events;

    /** 主线程ID 。*/
    pthread_t leader;

    /** 看门狗活动时间(毫秒)。*/
    time_t watchdog;

    /** 看门狗活动间隔(毫秒)。*/
    time_t interval;

    /** epoll句柄。 */
    int efd;
    
} good_mux_t;

/**
 * 多路复用器的节点。
*/
typedef struct _good_mux_node
{
    /** 句柄。*/
    int fd;

    /** 状态。 */
    int stable;

    /** 注册事件。*/
    uint32_t event_mark;

    /** 已分派事件。*/
    uint32_t event_disp;

    /** 引用计数。*/
    int refcount;

    /** 活动时间(毫秒)。*/
    time_t active;

    /** 超时(毫秒)。*/
    time_t timeout;

    /** 是否第一次注册。*/
    int mark_first;
    
    /** 是否第一次添加。*/
    int add_first;

}good_mux_node;


void good_mux_free(good_mux_t **ctx)
{
    good_mux_t *ctx_p;

    if(!ctx || !*ctx)
        return;

    ctx_p = *ctx;

    good_closep(&ctx_p->efd);
    good_pool_destroy(&ctx_p->events);
    good_map_destroy(&ctx_p->nodes);
    good_mutex_destroy(&ctx_p->mutex);

    /*free.*/
    good_heap_free(ctx_p);

    /*Set to NULL(0).*/
    *ctx = NULL;
}

good_mux_t *good_mux_alloc()
{
    int efd = -1;
    good_mux_t *ctx = NULL;
    
    efd = good_epoll_create();
    if (efd < 0)
        goto final_error;

    ctx = good_heap_alloc(sizeof(good_mux_t));
    if(!ctx)
        goto final_error;

    ctx->efd = efd;
    good_pool_init(&ctx->events,sizeof(good_epoll_event),20);
    good_map_init(&ctx->nodes,100);
    good_mutex_init2(&ctx->mutex,0);
    ctx->interval = 5000;
    ctx->watchdog = good_time_clock2kind_with(CLOCK_MONOTONIC,3);
    ctx->leader = 0;

    return ctx;

final_error:

    good_closep(&efd);
    good_heap_free(ctx);

    return NULL;
}

int good_mux_detach(good_mux_t *ctx,int fd)
{
    good_allocator_t *p = NULL;
    good_mux_node *node = NULL;
    int chk = 0;

    assert(ctx != NULL && fd >= 0);

    good_mutex_lock(&ctx->mutex,0);

    p = good_map_find(&ctx->nodes, &fd, sizeof(fd), sizeof(good_mux_node));
    if(!p)
        goto final_error;

    node = (good_mux_node *)p->pptrs[GOOD_MAP_VALUE];

    if (node->refcount > 0)
        GOOD_ERRNO_AND_GOTO1(EBUSY, final_error);

    good_epoll_drop(ctx->efd,fd);

    good_map_remove(&ctx->nodes, &fd, sizeof(fd));

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    good_mutex_unlock(&ctx->mutex);

    return chk;
}

int good_mux_attach(good_mux_t *ctx,int fd,time_t timeout)
{
    good_allocator_t *p = NULL;
    good_mux_node *node = NULL;
    int chk = 0;

    assert(ctx != NULL && fd >= 0);

    good_mutex_lock(&ctx->mutex,0);

    p = good_map_find(&ctx->nodes, &fd, sizeof(fd), sizeof(good_mux_node));
    if(!p)
        goto final_error;

    node = (good_mux_node *)p->pptrs[GOOD_MAP_VALUE];

    if (node->add_first != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL,final_error);

    node->fd = fd;
    node->timeout = timeout;
    node->stable = 1;
    node->active = good_time_clock2kind_with(CLOCK_MONOTONIC,3);
    node->mark_first = 1;
    node->add_first = 1;
    node->event_mark = node->event_disp = 0;
    node->refcount = 0;

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    good_mutex_unlock(&ctx->mutex);

    return chk;
}