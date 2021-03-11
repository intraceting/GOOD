/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "deuqe.h"

good_deque_t *good_deque_scroll(good_deque_t *deque, good_deque_t *self,int front)
{
    if (!deque)
        return NULL;

    if (self)
    {
        assert(deque = good_tree_father(self));

        if (front)
            return good_tree_sibling(self, 1);

        return good_tree_sibling(self, 0);
    }
    else
    {
        if (front)
            return good_tree_child(deque,1);

        return good_tree_child(deque,0);
    }
}

void good_deque_detach(good_deque_t *deque, good_deque_t *self)
{
    if (!deque || !self)
        return ;

    assert(deque = good_tree_father(self));

    good_tree_detach(self);

}

good_deque_t *good_deque_pop(good_deque_t *deque,int back)
{
    good_deque_t* node = NULL;

    if (!deque)
        return NULL;
    
    if(back)
        node = good_deque_scroll(deque,NULL,0);
    else 
        node = good_deque_scroll(deque,NULL,1);

    good_tree_detach(node);
    
    return node;
}

void good_deque_insert(good_deque_t *deque, good_deque_t *self,good_deque_t *where)
{
    if (!deque || !self)
        return ;

    if(where)
        assert(deque = good_tree_father(where));

    good_tree_insert(deque,self,where);
}

void good_deque_push(good_deque_t *deque, good_deque_t *self, int front)
{
    good_deque_t *where = (front ? good_deque_scroll(deque, NULL, 1) : NULL);

    good_deque_insert(deque, self, where);
}

void good_deque_clear(good_deque_t *deque,void (*free_cb)(good_deque_t *node, void *opaque), void *opaque)
{
    good_tree_clear(deque,free_cb,opaque);
}

void good_deque_clear2(good_deque_t *deque)
{
    good_deque_clear(deque,NULL,NULL);
}

void good_deque_free(good_deque_t **self)
{
    good_tree_free(self);
}

good_deque_t *good_deque_alloc(size_t size)
{
    return good_tree_alloc(size);
}