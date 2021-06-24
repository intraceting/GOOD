/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "mux.h"

/**
 * 多路复用器。
*/
typedef struct _abtk_mux
{
    /** epoll句柄。 >= 0 有效。*/
    int efd;

    /** 互斥量。*/
    abtk_mutex_t mutex;

    /** 节点表。*/
    abtk_map_t node_map;

    /** 事件池。*/
    abtk_pool_t event_pool;

    /** WAIT主线程ID。*/
    volatile pthread_t wait_leader;

    /** 看门狗活动时间(毫秒)。*/
    time_t watchdog_active;

    /** 看门狗活动间隔(毫秒)。*/
    time_t watchdog_intvl;

    /** 广播事件。*/
    uint32_t broadcast_want;
    
} abtk_mux_t;

/**
 * 多路复用器的节点。
*/
typedef struct _abtk_mux_node
{
    /** 句柄。>= 0 有效。*/
    int fd;

    /** 状态。!0 正常，0 异常。 */
    int stable;

    /** 注册事件。*/
    uint32_t event_mark;

    /** 分派事件。*/
    uint32_t event_disp;

    /** 引用计数。*/
    int refcount;

    /** 活动时间(毫秒)。*/
    time_t active;

    /** 超时(毫秒)。*/
    time_t timeout;

    /** 是否第一次注册。!0 是，0 否。*/
    int mark_first;
    
    /** 是否第一次添加。!0 是，0 否。*/
    int add_first;

}abtk_mux_node;


void abtk_mux_free(abtk_mux_t **ctx)
{
    abtk_mux_t *ctx_p;

    if(!ctx || !*ctx)
        return;

    ctx_p = *ctx;

    abtk_closep(&ctx_p->efd);
    abtk_pool_destroy(&ctx_p->event_pool);
    abtk_map_destroy(&ctx_p->node_map);
    abtk_mutex_destroy(&ctx_p->mutex);

    /*free.*/
    abtk_heap_free(ctx_p);

    /*Set to NULL(0).*/
    *ctx = NULL;
}

abtk_mux_t *abtk_mux_alloc()
{
    int efd = -1;
    abtk_mux_t *ctx = NULL;
    
    efd = abtk_epoll_create();
    if (efd < 0)
        goto final_error;

    ctx = abtk_heap_alloc(sizeof(abtk_mux_t));
    if(!ctx)
        goto final_error;

    ctx->efd = efd;
    abtk_pool_init(&ctx->event_pool, sizeof(abtk_epoll_event), 4 * 20 + 20);
    abtk_map_init(&ctx->node_map,400);
    abtk_mutex_init2(&ctx->mutex,0);
    ctx->watchdog_intvl = 5000;
    ctx->watchdog_active = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3);
    ctx->wait_leader = 0;

    return ctx;

final_error:

    abtk_closep(&efd);
    abtk_heap_free(ctx);

    return NULL;
}

int abtk_mux_detach(abtk_mux_t *ctx,int fd)
{
    abtk_allocator_t *p = NULL;
    abtk_mux_node *node = NULL;
    int chk = 0;

    assert(ctx != NULL && fd >= 0);

    abtk_mutex_lock(&ctx->mutex,1);

    p = abtk_map_find(&ctx->node_map, &fd, sizeof(fd),0);
    if(!p)
        goto final_error;

    node = (abtk_mux_node *)p->pptrs[ABTK_MAP_VALUE];

    if (node->refcount > 0)
        ABTK_ERRNO_AND_GOTO1(EBUSY, final_error);

    abtk_epoll_drop(ctx->efd,fd);

    abtk_map_remove(&ctx->node_map, &fd, sizeof(fd));

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    abtk_mutex_unlock(&ctx->mutex);

    return chk;
}

int abtk_mux_attach(abtk_mux_t *ctx,int fd,time_t timeout)
{
    abtk_allocator_t *p = NULL;
    abtk_mux_node *node = NULL;
    int chk = 0;

    assert(ctx != NULL && fd >= 0);

    abtk_mutex_lock(&ctx->mutex,1);

    p = abtk_map_find(&ctx->node_map, &fd, sizeof(fd), sizeof(abtk_mux_node));
    if(!p)
        goto final_error;

    node = (abtk_mux_node *)p->pptrs[ABTK_MAP_VALUE];

    if (node->add_first != 0)
        ABTK_ERRNO_AND_GOTO1(EINVAL,final_error);

    node->fd = fd;
    node->timeout = timeout;
    node->stable = 1;
    node->active = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3);
    node->mark_first = 1;
    node->add_first = 1;
    node->event_mark = node->event_disp = 0;
    node->refcount = 0;

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    abtk_mutex_unlock(&ctx->mutex);

    return chk;
}

static void _abtk_mux_disp(abtk_mux_t *ctx, abtk_mux_node *node, uint32_t event)
{
    abtk_epoll_event disp = {0};

    /*如果有错误，记录到节点上。*/
    if (event & ABTK_EPOLL_ERROR)
        node->stable = 0;

    if (node->stable)
    {
        /*在已注册事件中，排除已被分派的事件才是当前线程需要返回的。*/
        disp.events = ((event & node->event_mark) & (~node->event_disp));
    }
    else
    {
        /*如果发生错误，并且计数器为0，返回出错事件。*/
        if (node->refcount <= 0)
            disp.events = ABTK_EPOLL_ERROR;
    }

    /*根据发生的事件增加计数器。*/
    if (disp.events & ABTK_EPOLL_ERROR)
        node->refcount += 1;
    if (disp.events & ABTK_EPOLL_INPUT)
        node->refcount += 1;
    if (disp.events & ABTK_EPOLL_INOOB)
        node->refcount += 1;
    if (disp.events & ABTK_EPOLL_OUTPUT)
        node->refcount += 1;

    /*在节点上附加本次分派的事件。*/
    node->event_disp |= disp.events;

    /*清除即将通知的事件，注册事件只通知一次。*/
    node->event_mark &= ~disp.events;

    /*有事件时再推送到活动队列。*/
    if (disp.events)
    {
        disp.data.fd = node->fd;
        abtk_pool_push(&ctx->event_pool,&disp,sizeof(disp));
    }   
}

static void _abtk_mux_mark(abtk_mux_t *ctx, abtk_mux_node *node, uint32_t want, uint32_t done)
{
    abtk_epoll_event tmp = {0};

    /*清除分派的事件。*/
    node->event_disp &= ~done;

    /*绑定关注的事件，如果事件没有被激活，这里需要继续绑定。*/
    node->event_mark |= want;

    /*如果是第一次注册。*/
    if (node->mark_first)
    {
        /*这里设置为非阻塞*/
        if (abtk_fflag_add(node->fd, O_NONBLOCK) != 0)
            node->stable = 0;
    }

    /*如果未发生错误，进入正常流程。*/
    if (node->stable)
    {
        tmp.events = node->event_mark;
        tmp.data.fd = node->fd;

        if (abtk_epoll_mark(ctx->efd,node->fd,&tmp,node->mark_first) != 0)
            node->stable = 0;
        
        /*无论是否成功，第一次注册都已经完成。*/
        node->mark_first = 0;

        /*更节点新活动时间。*/
        node->active = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3);
    }

    /*
     * 1：如果发生错误，进入异常流程。
     * 2：如果当前处理的事件包括ERROR事件，则不用再次发出通知。
    */
    if (!node->stable && !(done & ABTK_EPOLL_ERROR))
        _abtk_mux_disp(ctx, node, ABTK_EPOLL_ERROR);

}

static int _abtk_mux_mark_scan_cb(abtk_allocator_t *alloc, void *opaque)
{
    abtk_mux_t *ctx = (abtk_mux_t *)opaque;
    abtk_mux_node *node = (abtk_mux_node *)alloc->pptrs[ABTK_MAP_VALUE];

    _abtk_mux_mark(ctx,node,ctx->broadcast_want,0);

    return 1;
}

int abtk_mux_mark(abtk_mux_t *ctx, int fd, uint32_t want, uint32_t done)
{
    abtk_allocator_t *p = NULL;
    abtk_mux_node *node = NULL;

    int chk = 0;

    assert(ctx != NULL);
    assert((want & ~(ABTK_EPOLL_INPUT | ABTK_EPOLL_INOOB | ABTK_EPOLL_OUTPUT | ABTK_EPOLL_ERROR)) == 0);
    assert((done & ~(ABTK_EPOLL_INPUT | ABTK_EPOLL_INOOB | ABTK_EPOLL_OUTPUT | ABTK_EPOLL_ERROR)) == 0);

    abtk_mutex_lock(&ctx->mutex,1);

    if (fd >= 0)
    {
        p = abtk_map_find(&ctx->node_map, &fd, sizeof(fd), 0);
        if (!p)
            goto final_error;

        node = (abtk_mux_node *)p->pptrs[ABTK_MAP_VALUE];

        _abtk_mux_mark(ctx, node, want, done);
    }
    else
    {
        /*Set.*/
        ctx->broadcast_want = want;

        /*遍历。*/
        ctx->node_map.dump_cb = _abtk_mux_mark_scan_cb;
        ctx->node_map.opaque = ctx;
        abtk_map_scan(&ctx->node_map);

        /*Clear.*/
        ctx->broadcast_want = 0;
    }

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    abtk_mutex_unlock(&ctx->mutex);

    return chk;   
}

static int _abtk_mux_watchdog_scan_cb(abtk_allocator_t *alloc, void *opaque)
{
    abtk_mux_t *ctx = (abtk_mux_t *)opaque;
    abtk_mux_node *node = (abtk_mux_node *)alloc->pptrs[ABTK_MAP_VALUE];

    /*负值或零，不启用超时检查。*/
    if (node->timeout <= 0)
        goto final;

    /*当事件队列排队过长时，中断看门狗检查，优先处理队列中的事件。*/
    if (ctx->event_pool.count >= 20)
        return -1;

    /*如果超时，派发ERROR事件。*/
    if ((ctx->watchdog_active - node->active) >= node->timeout)
        _abtk_mux_disp(ctx, node, ABTK_EPOLL_ERROR);

final:

    return 1;
}

static void _abtk_mux_watchdog(abtk_mux_t *ctx)
{
    uint64_t current = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3);

    /*看门狗活动间隔时间不能太密集。*/
    if ((current - ctx->watchdog_active) < ctx->watchdog_intvl)
        return;

    /*更新看门狗活动时间。*/
    ctx->watchdog_active = current;

    /*遍历。*/
    ctx->node_map.dump_cb = _abtk_mux_watchdog_scan_cb;
    ctx->node_map.opaque = ctx;
    abtk_map_scan(&ctx->node_map);

}

static void _abtk_mux_wait_disp(abtk_mux_t *ctx,abtk_epoll_event *events,int count)
{
    abtk_epoll_event *e;
    abtk_allocator_t *p;
    abtk_mux_node *node;

    for (int i = 0; i < count; i++)
    {
        e = &events[i];
        p = abtk_map_find(&ctx->node_map, &e->data.fd,sizeof(e->data.fd), 0);

        /*有那么一瞬间，当前返回的事件并不在(可能被分离)锁保护范围内的，因此这要做些处理。*/
        if (p == NULL)
            continue;
            
        node = (abtk_mux_node *)p->pptrs[ABTK_MAP_VALUE];

        /*派发事件。*/
        _abtk_mux_disp(ctx,node,e->events);

        /*更节点新活动时间*/
        node->active = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3);
    }
}

static uint64_t _abtk_mux_difference_timeout(uint64_t begin,uint64_t timeout)
{
    uint64_t span = UINT64_MAX;

    if(timeout >= 0)
    {
        span = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3) - begin;

        return timeout - span;
    }

    return span;
}

int abtk_mux_wait(abtk_mux_t *ctx,abtk_epoll_event *event,time_t timeout)
{
    abtk_epoll_event w[20];
    uint64_t begin = abtk_time_clock2kind_with(CLOCK_MONOTONIC,3);
    uint64_t remaining = 0;
    int count;
    int chk = 0;

    assert(ctx != NULL && event != NULL);
    
    abtk_mutex_lock(&ctx->mutex,1);

try_again:

    /*优先从事件队列中拉取。*/
    chk = abtk_pool_pull(&ctx->event_pool, event, sizeof(*event));
    if (chk >= 0)
        goto final;

    /*计算剩余超时时长。*/
    remaining = _abtk_mux_difference_timeout(begin, timeout);
    if (remaining <= 0)
        ABTK_ERRNO_AND_GOTO1(EINTR,final_error);

    /*多线程选主，只能有一个线程进入IO等待，其它线程等待事件通知。*/
    if(abtk_thread_leader_test(&ctx->wait_leader)==0)
    {
        /*通过看门狗检测长期不活动的节点。*/
        _abtk_mux_watchdog(ctx);

        /*唤醒其它线程，处理看门狗。*/
        abtk_mutex_signal(&ctx->mutex,1);

        /*解锁，使其它接口被访问。*/
        abtk_mutex_unlock(&ctx->mutex);

        /*IO等待。*/
        count = abtk_epoll_wait(ctx->efd,w,ABTK_ARRAY_SIZE(w),ABTK_MIN(remaining,ctx->watchdog_intvl));

        /*加锁，禁其它接口被访问。*/
        abtk_mutex_lock(&ctx->mutex,1);

        /*处理活动事件。*/
        _abtk_mux_wait_disp(ctx,w,count);

        /*唤醒其它线程，处理事件。*/
        abtk_mutex_signal(&ctx->mutex,1);

        /*主线程退出。*/
        abtk_thread_leader_quit(&ctx->wait_leader);
        
    }
    else
    {   
        /*等待主线程的通知，或超时退出。*/
        abtk_mutex_wait(&ctx->mutex,remaining);
    }

    /*No error, no event, try again.*/
    goto try_again;

final_error:

    chk = -1;

final:

    abtk_mutex_unlock(&ctx->mutex);

    return chk; 
}

int abtk_mux_unref(abtk_mux_t *ctx,abtk_epoll_event *event)
{
    abtk_allocator_t *p = NULL;
    abtk_mux_node *node = NULL;
    abtk_epoll_event tmp = {0};
    int chk = 0;

    assert(ctx != NULL && event != NULL);

    assert(event->data.fd >= 0);
    assert((event->events & ~(ABTK_EPOLL_INPUT | ABTK_EPOLL_INOOB | ABTK_EPOLL_OUTPUT | ABTK_EPOLL_ERROR)) == 0);

    abtk_mutex_lock(&ctx->mutex,1);

    p = abtk_map_find(&ctx->node_map, &event->data.fd, sizeof(event->data.fd),0);
    if(!p)
        goto final_error;

    node = (abtk_mux_node *)p->pptrs[ABTK_MAP_VALUE];

    /*无论成功或失败，记数器都要相应的减少，不然无法释放。*/
    if (event->events & ABTK_EPOLL_ERROR)
        node->refcount -= 1;
    if (event->events & ABTK_EPOLL_INPUT)
        node->refcount -= 1;
    if (event->events & ABTK_EPOLL_INOOB)
        node->refcount -= 1;
    if (event->events & ABTK_EPOLL_OUTPUT)
        node->refcount -= 1;

    /*
     * 1：如果发生错误，进入异常流程。
     * 2：如果当前处理的事件包括ERROR事件，则不用再次发出通知。
    */
    if (!node->stable && !(event->events & ABTK_EPOLL_ERROR))
        _abtk_mux_disp(ctx, node, ABTK_EPOLL_ERROR);

    /*No error.*/
    goto final;

final_error:

    chk = -1;

final:

    abtk_mutex_unlock(&ctx->mutex);

    return chk; 
}