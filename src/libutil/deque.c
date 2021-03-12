/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "deque.h"

good_deque_node *good_deque_scroll(good_deque_t *deque,good_deque_node *node,int front)
{
    good_tree_t* tree_p;

    if (!deque)
        return NULL;

    tree_p = &deque->root;

    if (node)
    {
        assert(good_tree_father(node) == tree_p);

        if (front)
            return good_tree_sibling(node, 1);

        return good_tree_sibling(node, 0);
    }
    else
    {
        if (front)
            return good_tree_child(tree_p,1);

        return good_tree_child(tree_p,0);
    }
}

void good_deque_unlink(good_deque_t *deque, good_deque_node *node)
{
    good_tree_t* tree_p;

    if (!deque || !node)
        return ;

    tree_p = &deque->root;

    assert(good_tree_father(node) == tree_p);

    good_tree_unlink(node);
}

void good_deque_insert(good_deque_t *deque, good_deque_node *node,good_deque_node *where)
{
    good_tree_t* tree_p;

    if (!deque || !node)
        return ;

    tree_p = &deque->root;

    if(where)
        assert(good_tree_father(where) == tree_p);

    good_tree_insert(tree_p,node,where);
}

good_deque_node *good_deque_pop(good_deque_t *deque,int back)
{
    good_deque_node* node = NULL;

    if (!deque)
        return NULL;
    
    if(back)
        node = good_deque_scroll(deque,NULL,0);
    else 
        node = good_deque_scroll(deque,NULL,1);

    good_tree_unlink(node);
    
    return node;
}

void good_deque_push(good_deque_t *deque, good_deque_node *node, int front)
{
    good_deque_node *where = NULL;

    if (!deque)
        return;

    where = (front ? good_deque_scroll(deque, NULL, 1) : NULL);

    good_deque_insert(deque, node, where);
}

/*
* 避免与TREE删除操作不一致。
*/
void _deque_clear_default_free(good_deque_node *node, void *opaque)
{
    good_deque_free(&node);
}

void good_deque_clear(good_deque_t *deque,void (*free_cb)(good_deque_node *node, void *opaque), void *opaque)
{
    good_tree_t* tree_p;

    if (!deque)
        return ;

    tree_p = &deque->root;

    good_tree_clear(tree_p,free_cb?free_cb:_deque_clear_default_free,opaque);
}

void good_deque_clear2(good_deque_t *deque)
{
    good_deque_clear(deque,NULL,NULL);
}

void good_deque_free(good_deque_node **node)
{
    good_tree_free(node);
}

good_deque_node *good_deque_alloc(size_t size)
{
    return good_tree_alloc(size);
}