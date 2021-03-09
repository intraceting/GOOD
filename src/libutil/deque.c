/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "deque.h"

void good_deque_detach(good_deque_t *deque, good_deque_t *node)
{
    if (!deque || !node)
        return;

    /*
     * 头<--->NODE<--->尾
     * 头<--->NODE
     * NODE<--->尾
     */
    if (node->chain[GOOD_DEQUE_CHAIN_NEXT])
        node->chain[GOOD_DEQUE_CHAIN_NEXT]->chain[GOOD_DEQUE_CHAIN_PREV] = node->chain[GOOD_DEQUE_CHAIN_PREV];
    if (node->chain[GOOD_DEQUE_CHAIN_PREV])
        node->chain[GOOD_DEQUE_CHAIN_PREV]->chain[GOOD_DEQUE_CHAIN_NEXT] = node->chain[GOOD_DEQUE_CHAIN_NEXT];

    /*
     * NODE是头?
    */
    if (node == deque->chain[GOOD_DEQUE_CHAIN_HEAD])
    {
        deque->chain[GOOD_DEQUE_CHAIN_HEAD] = node->chain[GOOD_DEQUE_CHAIN_NEXT];
        if (deque->chain[GOOD_DEQUE_CHAIN_HEAD])
            deque->chain[GOOD_DEQUE_CHAIN_HEAD]->chain[GOOD_DEQUE_CHAIN_PREV] = NULL;
    }

    /*
     * NODE是尾?
    */
    if (node == deque->chain[GOOD_DEQUE_CHAIN_TAIL])
    {
        deque->chain[GOOD_DEQUE_CHAIN_TAIL] = node->chain[GOOD_DEQUE_CHAIN_PREV];
        if (deque->chain[GOOD_DEQUE_CHAIN_TAIL])
            deque->chain[GOOD_DEQUE_CHAIN_TAIL]->chain[GOOD_DEQUE_CHAIN_NEXT] = NULL;
    }

    /*
     * 清空节点链。
    */
    node->chain[GOOD_DEQUE_CHAIN_NEXT] = NULL;
    node->chain[GOOD_DEQUE_CHAIN_PREV] = NULL;
}

void good_deque_insert(good_deque_t *deque, good_deque_t *node, good_deque_t *where)
{
    if (!deque || !node)
        return;

    /* 
     * 清空节点链。
    */
    node->chain[GOOD_DEQUE_CHAIN_NEXT] = NULL;
    node->chain[GOOD_DEQUE_CHAIN_PREV] = NULL;

    if(where)
    {
        if (where == deque->chain[GOOD_DEQUE_CHAIN_HEAD])
        {
            /*添加到头节点之前。*/
            where->chain[GOOD_DEQUE_CHAIN_PREV] = node;
            node->chain[GOOD_DEQUE_CHAIN_NEXT] = where;

            /*新的头节点。*/
            deque->chain[GOOD_DEQUE_CHAIN_HEAD] = node;
        }
        else
        {
            /*添加到节点之前*/
            where->chain[GOOD_DEQUE_CHAIN_PREV]->chain[GOOD_DEQUE_CHAIN_NEXT] = node;
            node->chain[GOOD_DEQUE_CHAIN_PREV] = where->chain[GOOD_DEQUE_CHAIN_PREV];
            node->chain[GOOD_DEQUE_CHAIN_NEXT] = where;
            where->chain[GOOD_DEQUE_CHAIN_PREV] = node;
        }
    }
    else
    {
        if (deque->chain[GOOD_DEQUE_CHAIN_TAIL])
        {
            /*添加到尾节点之后。*/
            deque->chain[GOOD_DEQUE_CHAIN_TAIL]->chain[GOOD_DEQUE_CHAIN_NEXT] = node;
            node->chain[GOOD_DEQUE_CHAIN_PREV] = deque->chain[GOOD_DEQUE_CHAIN_TAIL];

            /*新的尾节点。*/
            deque->chain[GOOD_DEQUE_CHAIN_TAIL] = node;
        }
        else
        {
            /*空链表，添加第一个结节。*/
            deque->chain[GOOD_DEQUE_CHAIN_TAIL] = deque->chain[GOOD_DEQUE_CHAIN_HEAD] = node;
        }
    }
}
