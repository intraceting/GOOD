/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tree.h"


good_tree_t *good_tree_father(good_tree_t *root)
{
    if (!root)
        return NULL;

    return root->chain[GOOD_TREE_CHAIN_FATHER];
}

good_tree_t *good_tree_prev(good_tree_t *root)
{
    if (!root)
        return NULL;

    return root->chain[GOOD_TREE_CHAIN_PREV];
}

good_tree_t *good_tree_next(good_tree_t *root)
{
    if (!root)
        return NULL;

    return root->chain[GOOD_TREE_CHAIN_NEXT];
}

good_tree_t *good_tree_head(good_tree_t *root)
{
    if (!root)
        return NULL;

    return root->chain[GOOD_TREE_CHAIN_HEAD];
}

good_tree_t *good_tree_tail(good_tree_t *root)
{
    if (!root)
        return NULL;

    return root->chain[GOOD_TREE_CHAIN_TAIL];
}

void good_tree_detach(good_tree_t *node)
{
    good_tree_t *root = NULL;

    if (!node)
        return;

    /*
     * 获取父节点
     */
    root = node->chain[GOOD_TREE_CHAIN_FATHER];

    if (!root)
        return;

    /*
     * 首<--->NODE<--->尾
     * 首<--->NODE
     * NODE<--->尾
     */
    if (node->chain[GOOD_TREE_CHAIN_NEXT])
        node->chain[GOOD_TREE_CHAIN_NEXT]->chain[GOOD_TREE_CHAIN_PREV] = node->chain[GOOD_TREE_CHAIN_PREV];
    if (node->chain[GOOD_TREE_CHAIN_PREV])
        node->chain[GOOD_TREE_CHAIN_PREV]->chain[GOOD_TREE_CHAIN_NEXT] = node->chain[GOOD_TREE_CHAIN_NEXT];

    /*
     * NODE 是首?
    */
    if (node == root->chain[GOOD_TREE_CHAIN_HEAD])
    {
        root->chain[GOOD_TREE_CHAIN_HEAD] = node->chain[GOOD_TREE_CHAIN_NEXT];
        if (root->chain[GOOD_TREE_CHAIN_HEAD])
            root->chain[GOOD_TREE_CHAIN_HEAD]->chain[GOOD_TREE_CHAIN_PREV] = NULL;
    }

    /*
     * NODE 是尾?
    */
    if (node == root->chain[GOOD_TREE_CHAIN_TAIL])
    {
        root->chain[GOOD_TREE_CHAIN_TAIL] = node->chain[GOOD_TREE_CHAIN_PREV];
        if (root->chain[GOOD_TREE_CHAIN_TAIL])
            root->chain[GOOD_TREE_CHAIN_TAIL]->chain[GOOD_TREE_CHAIN_NEXT] = NULL;
    }

    /*
     * 打断与父节点的关系链，但同时保留子节点关系链。
    */
    node->chain[GOOD_TREE_CHAIN_FATHER] = NULL;
    node->chain[GOOD_TREE_CHAIN_NEXT] = NULL;
    node->chain[GOOD_TREE_CHAIN_PREV] = NULL;
}

void good_tree_insert(good_tree_t *root, good_tree_t *node, good_tree_t *where)
{
    if (!root || !node)
        return;

    /* 
     * 绑定新父节点，并且打断旧的关系链。
    */
    node->chain[GOOD_TREE_CHAIN_FATHER] = root;
    node->chain[GOOD_TREE_CHAIN_NEXT] = NULL;
    node->chain[GOOD_TREE_CHAIN_PREV] = NULL;

    if (where)
    {
        assert(root = where->chain[GOOD_TREE_CHAIN_FATHER]);

        if (where == root->chain[GOOD_TREE_CHAIN_HEAD])
        {
            /*
             * 添加到头节点之前。
            */
            where->chain[GOOD_TREE_CHAIN_PREV] = node;
            node->chain[GOOD_TREE_CHAIN_NEXT] = where;

            /* 
             * 新的头节点。
            */
            root->chain[GOOD_TREE_CHAIN_HEAD] = node;
        }
        else
        {
            /*
             * 添加到节点之前
            */
            where->chain[GOOD_TREE_CHAIN_PREV]->chain[GOOD_TREE_CHAIN_NEXT] = node;
            node->chain[GOOD_TREE_CHAIN_PREV] = where->chain[GOOD_TREE_CHAIN_PREV];
            node->chain[GOOD_TREE_CHAIN_NEXT] = where;
            where->chain[GOOD_TREE_CHAIN_PREV] = node;
        }
    }
    else
    {
        if (root->chain[GOOD_TREE_CHAIN_TAIL])
        {
            /*
             * 添加到尾节点之后。
            */
            root->chain[GOOD_TREE_CHAIN_TAIL]->chain[GOOD_TREE_CHAIN_NEXT] = node;
            node->chain[GOOD_TREE_CHAIN_PREV] = root->chain[GOOD_TREE_CHAIN_TAIL];

            /* 
             * 新的尾节点。
            */
            root->chain[GOOD_TREE_CHAIN_TAIL] = node;
        }
        else
        {
            /*
             * 空链表，添加第一个节点。
            */
            root->chain[GOOD_TREE_CHAIN_TAIL] = root->chain[GOOD_TREE_CHAIN_HEAD] = node;
        }
    }
}

good_tree_t *good_tree_pop_head(good_tree_t *root)
{
    good_tree_t *node = NULL;

    if (!root)
        return NULL;

    node = good_tree_head(root);

    if (!node)
        return NULL;

    good_tree_detach(node);

    return node;
}

good_tree_t *good_tree_pop_tail(good_tree_t *root)
{
    good_tree_t *node = NULL;

    if (!root)
        return NULL;

    node = good_tree_tail(root);

    if (!node)
        return NULL;

    good_tree_detach(node);

    return node;
}

void good_tree_push_head(good_tree_t *root, good_tree_t *node)
{
    if (!root || !node)
        return;

    good_tree_insert(root, node, good_tree_head(root));
}

void good_tree_push_back(good_tree_t *root, good_tree_t *node)
{
    if (!root || !node)
        return;

    good_tree_insert(root, node, NULL);
}

void good_tree_clear(good_tree_t **root,void (*free_cb)(good_tree_t *node, void *opaque), void *opaque)
{
    good_tree_t *root_p = NULL;
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;

    if(!root || !*root)
        return;
    
    root_p = *root;

    /*
     * 先从树节点断开关系链，以防清理到父节点关系链。 
    */
    good_tree_detach(root_p);

    while (root_p)
    {
        node = good_tree_tail(root_p);

        if (node)
        {
            child = good_tree_tail(node);

            /*
             * 检测是否有子节点，如果有先清理子节点。
            */
            if (child)
            {
                root_p = node;
            }
            else
            {
                good_tree_detach(node);

                if(free_cb)
                {
                    free_cb(node,opaque);
                    node = NULL;
                }
                else 
                {
                    good_buffer_unref((void **)&node);
                }    
            }
        }
        else
        {
            /*
             * 没有子节点，返回到父节点。
            */
            root_p = good_tree_father(root_p);
        }
    }

    /*
     * 删除自己
    */
    if(free_cb)
        free_cb(root_p,opaque);
    else 
        good_buffer_unref((void **)&root_p);

    /*Must be set NULL(0)*/
    *root = NULL;

}

good_tree_t *good_tree_alloc(size_t size)
{
    good_tree_t *node = (good_tree_t *)good_buffer_alloc(sizeof(good_tree_t) + size,NULL,NULL);

    if (!node)
        return NULL;

    if (size > 0)
    {
        node->size = size;
        node->data = GOOD_PTR2PTR(void, node, sizeof(good_tree_t)); // NODE + 1
    }
    else
    {
        node->size = 0;
        node->data = NULL;
    }

    return node;
}
