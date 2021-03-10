/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_DEQUE_H
#define GOOD_UTIL_DEQUE_H

#include <stdint.h>
#include <stddef.h>

#include "macro.h"
#include "buffer.h"

/**
 * 链表
*/
typedef struct _good_deque
{
    /**
     * 链
    */
    struct _good_deque *chain[2];

    /**
    * 上
    */
#define GOOD_DEQUE_CHAIN_PREV 0

    /**
    * 下
    */
#define GOOD_DEQUE_CHAIN_NEXT 1

    /**
    * 首
    */
#define GOOD_DEQUE_CHAIN_HEAD 0

    /**
    * 尾
    */
#define GOOD_DEQUE_CHAIN_TAIL 1

    /**
     * 
    */
    size_t size;

    /**
     * 
    */
    void *data;

} good_deque_t;

/**
 * 链表节点分离
 * 
*/
void good_deque_detach(good_deque_t *deque, good_deque_t *node);

/**
 * 链表节点插入
 * 
 * @param node 新节点
 * @param where NULL(0) 新节点添加到末尾，!NULL(0) 新节点添加到'where'节点之前。
 * 
*/
void good_deque_insert(good_deque_t *deque, good_deque_t *node, good_deque_t *where);

/**
 * 
*/
static inline good_deque_t *good_deque_head(good_deque_t *deque)
{
    if (!deque)
        return NULL;

    return deque->chain[GOOD_DEQUE_CHAIN_HEAD];
}

/**
 * 
*/
static inline good_deque_t *good_deque_tail(good_deque_t *deque)
{
    if (!deque)
        return NULL;

    return deque->chain[GOOD_DEQUE_CHAIN_TAIL];
}

/**
 * 
*/
static inline good_deque_t *good_deque_prev(good_deque_t *node)
{
    if (!node)
        return NULL;

    return node->chain[GOOD_DEQUE_CHAIN_PREV];
}

/**
 * 
*/
static inline good_deque_t *good_deque_next(good_deque_t *node)
{
    if (!node)
        return NULL;

    return node->chain[GOOD_DEQUE_CHAIN_NEXT];
}

/**
 * 
*/
static inline good_deque_t *good_deque_pop_front(good_deque_t *deque)
{
    good_deque_t *node = NULL;

    if (!deque)
        return NULL;

    node = deque->chain[GOOD_DEQUE_CHAIN_HEAD];

    if (!node)
        return NULL;

    good_deque_detach(deque, node);

    return node;
}

/**
 * 
*/
static inline good_deque_t *good_deque_pop_back(good_deque_t *deque)
{
    good_deque_t *node = NULL;

    if (!deque)
        return NULL;

    node = deque->chain[GOOD_DEQUE_CHAIN_TAIL];

    if (!node)
        return NULL;

    good_deque_detach(deque, node);

    return node;
}

/**
 * 
*/
static inline void good_deque_push_front(good_deque_t *deque, good_deque_t *node)
{
    if (!deque || !node)
        return;

    good_deque_insert(deque, node, good_deque_head(deque));
}

/**
 * 
*/
static inline void good_deque_push_back(good_deque_t *deque, good_deque_t *node)
{
    if (!deque || !node)
        return;

    good_deque_insert(deque, node, NULL);
}

/**
 * 申请节点
 * 
 * @see good_buffer_alloc()
*/
static inline good_deque_t *good_deque_alloc(size_t size, void (*clean_cb)(void *buf, void *opaque), void *opaque)
{
    good_deque_t *node = (good_deque_t *)good_buffer_alloc(sizeof(good_deque_t) + size, clean_cb, opaque);

    if (!node)
        return NULL;

    if (size > 0)
    {
        node->size = size;
        node->data = GOOD_PTR2PTR(void, node, sizeof(good_deque_t)); // NODE + 1
    }
    else
    {
        node->size = 0;
        node->data = NULL;
    }

    return node;
}

/**
 * 申请节点
 * 
 * @see good_deque_alloc()
*/
static inline good_deque_t *good_deque_alloc2(size_t size)
{
    return good_deque_alloc(size, NULL, NULL);
}

/**
 * 链表节点默认的清理函数
 * 
*/
static inline void good_deque_default_clear(good_deque_t *deque)
{
    good_deque_t *node = NULL;

    if (!deque)
        return;

    while (1)
    {
        node = good_deque_pop_front(deque);

        if (!node)
            break;

        good_buffer_unref((void **)&node);
    }
}

#endif //GOOD_UTIL_DEQUE_H
