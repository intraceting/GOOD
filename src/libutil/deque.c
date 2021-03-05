/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "deque.h"

void good_deque_detach(good_deque_t *deque, good_deque_t *node)
{
    /*
     * 头<--->NODE<--->尾
     * 头<--->NODE
     * NODE<--->尾
     */
    if (node->chain.sibling.next)
        node->chain.sibling.next->chain.sibling.prev = node->chain.sibling.prev;
    if (node->chain.sibling.prev)
        node->chain.sibling.prev->chain.sibling.next = node->chain.sibling.next;

    /*
     * NODE是头?
    */
    if (node == deque->chain.child.head)
    {
        deque->chain.child.head = node->chain.sibling.next;
        if (deque->chain.child.head)
            deque->chain.child.head->chain.sibling.prev = NULL;
    }

    /*
     * NODE是尾?
    */
    if (node == deque->chain.child.tail)
    {
        deque->chain.child.tail = node->chain.sibling.prev;
        if (deque->chain.child.tail)
            deque->chain.child.tail->chain.sibling.next = NULL;
    }

    /*
     * 清空节点链。
    */
    node->chain.sibling.next = NULL;
    node->chain.sibling.prev = NULL;
}

void good_deque_insert(good_deque_t *deque, good_deque_t *node, good_deque_t *where)
{
    /* 
     * 清空节点链。
    */
    node->chain.sibling.next = NULL;
    node->chain.sibling.prev = NULL;

    if(where)
    {
        if (where == deque->chain.child.head)
        {
            /*添加到头节点之前。*/
            where->chain.sibling.prev = node;
            node->chain.sibling.next = where;

            /*新的头节点。*/
            deque->chain.child.head = node;
        }
        else
        {
            /*添加到节点之前*/
            where->chain.sibling.prev->chain.sibling.next = node;
            node->chain.sibling.prev = where->chain.sibling.prev;
            node->chain.sibling.next = where;
        }
    }
    else
    {
        if (deque->chain.child.tail)
        {
            /*添加到尾节点之后。*/
            deque->chain.child.tail->chain.sibling.next = node;
            node->chain.sibling.prev = deque->chain.child.tail;

            /*新的尾节点。*/
            deque->chain.child.tail = node;
        }
        else
        {
            /*空链表，添加第一个结节。*/
            deque->chain.child.tail = deque->chain.child.head = node;
        }
    }
}
