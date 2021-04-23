/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tree.h"


good_tree_t *good_tree_father(const good_tree_t *self)
{
    assert(self);

    return self->chain[GOOD_TREE_CHAIN_FATHER];
}

good_tree_t *good_tree_sibling(const good_tree_t *self,int elder)
{
    assert(self);

    if(elder)
        return self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
    
    return self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];
}


good_tree_t *good_tree_child(const good_tree_t *self,int first)
{
    assert(self);

    if(first)
        return self->chain[GOOD_TREE_CHAIN_CHILD_FIRST];

    return self->chain[GOOD_TREE_CHAIN_CHILD_LEAST];
}

void good_tree_unlink(good_tree_t *self)
{
    good_tree_t *root = NULL;

    assert(self);

    /* 获取父节点*/
    root = self->chain[GOOD_TREE_CHAIN_FATHER];

    if (!root)
        return;

    /*
     * 首<--->NODE<--->尾
     * 首<--->NODE
     * NODE<--->尾
     */
    if (self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT])
        self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
    if (self->chain[GOOD_TREE_CHAIN_SIBLING_PREV])
        self->chain[GOOD_TREE_CHAIN_SIBLING_PREV]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];

    /* NODE 是首?*/
    if (self == root->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
    {
        root->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];
        if (root->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
            root->chain[GOOD_TREE_CHAIN_CHILD_FIRST]->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;
    }

    /* NODE 是尾? */
    if (self == root->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
    {
        root->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
        if (root->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
            root->chain[GOOD_TREE_CHAIN_CHILD_LEAST]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    }

    /* 打断与父节点的关系链，但同时保留子节点关系链。*/
    self->chain[GOOD_TREE_CHAIN_FATHER] = NULL;
    self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    self->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;
}

void good_tree_insert(good_tree_t *father, good_tree_t *child, good_tree_t *where)
{
    assert(father && child);

    /*必须是根节点，或独立节点。 */
    assert(NULL == child->chain[GOOD_TREE_CHAIN_FATHER]);
    assert(NULL == child->chain[GOOD_TREE_CHAIN_SIBLING_PREV]);
    assert(NULL == child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]);

    /* 绑定新父节点。*/
    child->chain[GOOD_TREE_CHAIN_FATHER] = father;

    if (where)
    {
        assert(father = where->chain[GOOD_TREE_CHAIN_FATHER]);

        if (where == father->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
        {
            /*添加到头节点之前。*/
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = where;

            /* 新的头节点。*/
            father->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = child;
        }
        else
        {
            /*添加到节点之前*/
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = where->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
            child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = where;
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = child;
        }
    }
    else
    {
        if (father->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
        {
            /* 添加到尾节点之后。*/
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = father->chain[GOOD_TREE_CHAIN_CHILD_LEAST];

            /* 新的尾节点。*/
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = child;
        }
        else
        {
            /* 空链表，添加第一个节点。*/
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = father->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = child;
        }
    }
}

void good_tree_insert2(good_tree_t *father, good_tree_t *child,int first)
{
    good_tree_t* where = NULL;

    assert(father && child);

    if(first)
        where = good_tree_child(father,1);

    good_tree_insert(father,child,where);
}

void good_tree_swap(good_tree_t *src,good_tree_t *dst)
{
    good_tree_t *father = NULL;
    good_tree_t *src_next = NULL;
    good_tree_t *dst_next = NULL;

    assert(src && dst);
    assert(src != dst);
    assert(good_tree_father(src) && good_tree_father(dst));
    assert(good_tree_father(src) == good_tree_father(dst));

    father = good_tree_father(src);
    src_next = good_tree_sibling(src,0);
    dst_next = good_tree_sibling(dst,0);

    /* 两个兄弟紧挨着。*/
    if(src_next == dst)
    {
        good_tree_unlink(dst);
        good_tree_insert(father,dst,src);
    }
    else if(dst_next == src)
    {
        good_tree_unlink(src);
        good_tree_insert(father,src,dst);
    }
    else
    {
        /* 有其它兄弟姐妺夹在中间。 */
        good_tree_unlink(dst);
        good_tree_unlink(src);

        if(src_next)
            good_tree_insert(father,dst,src_next);
        else 
            good_tree_insert(father,dst,NULL);
        
        if(dst_next)
            good_tree_insert(father,src,dst_next);
        else
            good_tree_insert(father,src,NULL);
    }
}

void good_tree_free(good_tree_t **root)
{
    good_tree_t *root_p = NULL;
    good_tree_t *father = NULL;
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;

    if(!root || !*root)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    /* 复制一下 */
    root_p = *root;

    /* 以防清理到父和兄弟节点。 */
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_FATHER]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_SIBLING_PREV]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]);

    while (root_p)
    {
        node = good_tree_child(root_p,0);

        if (node)
        {
            child = good_tree_child(node,0);

            /* 检测是否有子节点，如果有先清理子节点。  */
            if (child)
            {
                root_p = node;
            }
            else
            {
                good_tree_unlink(node);

                good_allocator_unref(&node->alloc);
                good_heap_freep((void**)&node);
            }
        }
        else
        {
            /* 没有子节点，返回到父节点。*/
            root_p = good_tree_father(root_p);
        }
    }

    good_allocator_unref(&(*root)->alloc);
    good_heap_freep((void**)root);

}

good_tree_t *good_tree_alloc(good_allocator_t *alloc)
{
    good_tree_t *node = (good_tree_t *)good_heap_alloc(sizeof(good_tree_t));

    if (!node)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    node->alloc = alloc;

    return node;
}

good_tree_t *good_tree_alloc2(size_t *sizes, size_t numbers)
{
    good_tree_t *node = good_tree_alloc(NULL);
    good_allocator_t *alloc = good_allocator_alloc(sizes, numbers);

    if (!node || !alloc)
        goto final_error;

    node->alloc = alloc;
    
    return node;

final_error:

    /* 走到这里出错了。 */
    good_tree_free(&node);
    good_allocator_unref(&alloc);
    
    GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);
}

good_tree_t *good_tree_alloc3(size_t size)
{
    return good_tree_alloc2(&size,1);
}

void good_tree_scan(good_tree_t *root,good_tree_iterator_t* it)
{
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;
    good_tree_t **stack = NULL;
    size_t stack_size = PATH_MAX/2;
    size_t depth = 0;// begin 0
    int chk;

    assert(root != NULL && it != NULL);
    assert(it->dump_cb!= NULL);
        
    /* 如果调用者不确定，则在内部自动确定。  */
    if (it->depth_max > stack_size)
        stack_size = it->depth_max;

    stack = (good_tree_t **)good_heap_alloc(stack_size * sizeof(good_tree_t *));
    if (!stack)
        goto final;

    /* 根  */
    chk = it->dump_cb(0,root,it->opaque);
    if(chk == 0)
        goto final;

    /* 从第一个孩子开始遍历。 */
    node = good_tree_child(root,1);

    while(node)
    {
        chk = it->dump_cb(depth + 1, node, it->opaque);
        if (chk < 0)
            goto final;

        if(chk > 0)
            child = good_tree_child(node,1);
        else 
            child = NULL;

        if(child)
        {
            assert(stack_size > depth);

            stack[depth++] = node;

            node = child;
        }
        else
        {
            node = good_tree_sibling(node,0);

            while (!node && depth > 0)
            {
                node = stack[--depth];
                node = good_tree_sibling(node,0);
            }
        }
    }

final:

    good_heap_freep((void**)&stack);

}

ssize_t good_tree_fprintf(FILE* fp,size_t depth,const good_tree_t *node,const char* fmt,...)
{
    ssize_t wsize = 0;

    va_list vaptr;
    va_start(vaptr, fmt);

    wsize = good_tree_vfprintf(fp,depth,node,fmt,vaptr);

    va_end(vaptr);

    return wsize;
}

ssize_t good_tree_vfprintf(FILE* fp,size_t depth,const good_tree_t *node,const char* fmt,va_list args)
{
    good_tree_t *tmp = NULL;
    good_tree_t **stack = NULL;
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fp && node && fmt);

    if (depth <= 0)
    {
        wsize2 = vfprintf(fp,fmt,args);
        if (wsize2 <= 0)
            goto final;

        wsize += wsize2;
    }
    else
    {
        /*准备堆栈。 */
        stack = good_heap_alloc(depth * sizeof(good_tree_t *));
        if(!stack)
            GOOD_ERRNO_AND_RETURN1(ENOMEM,-1);

        tmp = (good_tree_t *)node;

        for (size_t i = 1; i < depth; i++)
            stack[depth-i] = (tmp = good_tree_father(tmp));

        for (size_t i = 1; i < depth; i++)
        {
            if (good_tree_sibling((good_tree_t *)stack[i], 0))
                wsize2 = fprintf(fp, "│   ");
            else
                wsize2 = fprintf(fp, "    ");

            if (wsize2 <= 0)
                goto final;

            wsize += wsize2;
        }

        if(good_tree_sibling(node,0))
            wsize2 = fprintf(fp,"├── ");
        else 
            wsize2 = fprintf(fp,"└── ");

        if (wsize2 <= 0)
            goto final;

        wsize += wsize2;

        wsize2 = vfprintf(fp,fmt,args);

        if (wsize2 <= 0)
            goto final;

        wsize += wsize2;
    }

final:

    good_heap_freep((void**)&stack);

    return wsize;
}

ssize_t good_tree_snprintf(char *buf, size_t max, size_t depth, const good_tree_t *node, const char *fmt, ...)
{
    ssize_t wsize = 0;

    assert(buf != NULL && max >0 && node != NULL && fmt != NULL);

    va_list vaptr;
    va_start(vaptr, fmt);

    wsize = good_tree_vsnprintf(buf,max,depth,node,fmt,vaptr);

    va_end(vaptr);
    
    return wsize;
}

ssize_t good_tree_vsnprintf(char *buf, size_t max, size_t depth, const good_tree_t *node,const char* fmt,va_list args)
{
    FILE* fp = NULL;
    ssize_t wsize = 0;

    assert(buf != NULL && max >0 && node != NULL && fmt != NULL);

    fp = fmemopen(buf,max,"w");
    if(!fp)
        return -1;

    wsize = good_tree_vfprintf(fp,depth,node,fmt,args);

    fclose(fp);
    
    return wsize;
}

void good_tree_sort(good_tree_t *father,good_tree_order_t *order)
{
    good_tree_t *t1 = NULL;
    good_tree_t *t2 = NULL;
    good_tree_t *t3 = NULL;
    int chk;

    assert(father != NULL && order != NULL);
    assert(order->compare_cb != NULL);

    t2 = good_tree_child(father, 1);
    while (t2)
    {
        t3 = good_tree_sibling(t1 = t2, 0);
        while (t3)
        {
            chk = order->compare_cb(t1,t3,order->opaque);

            if(order->by)
            {
                if (chk > 0)
                    t1 = t3;
            }
            else
            {
                if (chk < 0)
                    t1 = t3;
            }

            t3 = good_tree_sibling(t3, 0);
        }

        /*需要交换时，再进行交换。*/
        if (t1 != t2)
            good_tree_swap(t1, t2);

        t2 = good_tree_sibling(t1, 0);
    }
}