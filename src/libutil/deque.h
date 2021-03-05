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

/**
 * 链表
*/
typedef struct _good_deque
{
    /**
     * 链
    */
    union
    {
        /**
         * 兄弟
        */
        struct
        {
            /**
             * 兄
            */
            struct _good_deque *prev;

            /**
             * 弟
            */
            struct _good_deque *next;
        } sibling;

        /**
        * 孩子
        */
        struct
        {
            /**
             * 老大
            */
            struct _good_deque *head;

            /*
             * 老么
            */
            struct _good_deque *tail;
        } child;

    } chain;

    /**
     * 
    */
    size_t size;

    /**
     * 
    */
    void* data;

} good_deque_t;


/**
 * 链表节点剪切
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


#endif //GOOD_UTIL_DEQUE_H
