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
    good_pool_init(&ctx->events,sizeof(good_epoll_event),40);
    good_map_init(&ctx->nodes,400);
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

static void _good_mux_disp(good_mux_t *ctx, good_mux_node *node, uint32_t event)
{
    good_epoll_event disp = {0};

    /*如果有错误，记录到节点上。*/
    if (event & GOOD_EPOLL_ERROR)
        node->stable = 0;

    if (node->stable)
    {
        /*在已注册事件中，排除已被分派的事件才是当前线程需要返回的。*/
        disp.events = ((event & node->event_mark) & (~node->event_disp));
    }
    else
    {
        /*如果发生错误，并且没有已经分派的事件，返回出错事件。*/
        if (node->refcount <= 0)
            disp.events = GOOD_EPOLL_ERROR;
    }

    /*根据发生的事件增加计数器*/
    if (disp.events & GOOD_EPOLL_ERROR)
        node->refcount += 1;
    if (disp.events & GOOD_EPOLL_INPUT)
        node->refcount += 1;
    if (disp.events & GOOD_EPOLL_OUTPUT)
        node->refcount += 1;

    /*在节点上附加本次分派的事件。*/
    node->event_disp |= disp.events;

    /*清除即将通知的事件，注册事件只通知一次。*/
    node->event_mark &= ~disp.events;

    /*有事件时再推送到活动队列。*/
    if (disp.events)
        good_pool_push(&ctx->events,&disp,sizeof(disp));
}

int good_mux_mark(good_mux_t *ctx,int fd,uint32_t concerned,uint32_t completed)
{
    good_allocator_t *p = NULL;
    good_mux_node *node = NULL;
    good_epoll_event tmp = {0};
    int chk = 0;

    assert(ctx != NULL && fd >= 0);
    assert((concerned & ~(GOOD_EPOLL_INPUT | GOOD_EPOLL_INOOB | GOOD_EPOLL_OUTPUT)) == 0);
    assert((completed & ~(GOOD_EPOLL_INPUT | GOOD_EPOLL_INOOB | GOOD_EPOLL_OUTPUT | GOOD_EPOLL_ERROR)) == 0);

    good_mutex_lock(&ctx->mutex,0);

    p = good_map_find(&ctx->nodes, &fd, sizeof(fd), sizeof(good_mux_node));
    if(!p)
        goto final_error;

    node = (good_mux_node *)p->pptrs[GOOD_MAP_VALUE];

    /*清除分派的事件。*/
    node->event_disp &= ~concerned;

    /*绑定关注的事件，如果事件没有被激活，这里需要继续绑定。*/
    node->event_mark |= concerned;

    /*无论成功或失败，记数器都要相应的减少，不然无法释放。*/
    if (completed & GOOD_EPOLL_ERROR)
        node->refcount -= 1;
    if (completed & GOOD_EPOLL_INPUT)
        node->refcount -= 1;
    if (completed & GOOD_EPOLL_OUTPUT)
        node->refcount -= 1;

    /*如果是第一次注册。*/
    if (node->mark_first)
    {
        /*这里设置为非阻塞*/
        if (good_fflag_add(node->fd, O_NONBLOCK) != 0)
            node->stable = 0;
    }

    /*如果未发生错误，进入正常流程。*/
    if (node->stable)
    {
        tmp.events = node->event_mark;
        tmp.data.fd = node->fd;

        if (good_epoll_mark(ctx->efd,fd,&tmp,node->mark_first) != 0)
            node->stable = 0;
        
        /*无论是否成，第一次注册都已经完成。*/
        node->mark_first = 0;

        /*更节点新活动时间。*/
        node->active = good_time_clock2kind_with(CLOCK_MONOTONIC,3);
    }

    /*如果发生错误，进入异常流程。*/
    if (!node->stable)
    {
        /*如果当前处理的事件包括ERROR事件，则不用再次发出通知。*/
        if(!(completed & GOOD_EPOLL_ERROR))
        {
            /*分派一个出错事件。*/
            _good_mux_disp(ctx,node,GOOD_EPOLL_ERROR);
        }
    }

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    good_mutex_unlock(&ctx->mutex);

    return chk;   
}

static int _good_mux_watchdog_cb(good_allocator_t *alloc, void *opaque)
{
    good_mux_t *ctx = (good_mux_t *)opaque;
    good_mux_node *node = (good_mux_node *)alloc->pptrs[GOOD_MAP_VALUE];

    /*负值或零，不启用超时检测。*/
    if (node->timeout <= 0)
        goto final;

    /*如果超时，派发ERROR事件。*/
    if ((ctx->watchdog - node->active) >= node->timeout)
        _good_mux_disp(ctx, node, GOOD_EPOLL_ERROR);

final:

    return 1;
}

static void _good_mux_watchdog(good_mux_t *ctx)
{
    uint64_t current = good_time_clock2kind_with(CLOCK_MONOTONIC,3);

    /*看门狗活动间隔时间不能太密集。*/
    if ((current - ctx->watchdog) < ctx->interval)
        return;

    /*更新看门狗活动时间。*/
    ctx->watchdog = current;

    /*遍历。*/
    ctx->nodes.dump_cb = _good_mux_watchdog_cb;
    ctx->nodes.opaque = ctx;
    good_map_scan(&ctx->nodes);

}

static void _good_mux_wait_disp(good_mux_t *ctx,good_epoll_event *events,int count)
{
    good_epoll_event *e;
    good_allocator_t *p;
    good_mux_node *node;

    for (int i = 0; i < count; i++)
    {
        e = &events[i];
        p = good_map_find(&ctx->nodes, &e->data.fd,sizeof(e->data.fd), 0);

        /*有那么一瞬间，当前返回的事件并不在锁保护范围内的，因此这要做些处理。*/
        if (p == NULL)
            continue;
            
        node = (good_mux_node *)p->pptrs[GOOD_MAP_VALUE];

        /*派发事件。*/
        _good_mux_disp(ctx,node,e->events);

        /*更节点新活动时间*/
        node->active = good_time_clock2kind_with(CLOCK_MONOTONIC,3);
    }
}

static uint64_t _good_mux_difference_timeout(uint64_t begin,uint64_t timeout)
{
    uint64_t span = INTMAX_MAX;

    if(timeout >= 0)
    {
        span = good_time_clock2kind_with(CLOCK_MONOTONIC,3) - begin;

        return timeout - span;
    }

    return span;
}