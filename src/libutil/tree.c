/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tree.h"


good_tree_t *good_tree_father(const good_tree_t *self)
{
    if (!self)
        return NULL;

    return self->chain[GOOD_TREE_CHAIN_FATHER];
}

good_tree_t *good_tree_sibling(const good_tree_t *self,int elder)
{
    if (!self)
        return NULL;

    if(elder)
        return self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
    
    return self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];
}


good_tree_t *good_tree_child(const good_tree_t *self,int first)
{
    if (!self)
        return NULL;

    if(first)
        return self->chain[GOOD_TREE_CHAIN_CHILD_FIRST];

    return self->chain[GOOD_TREE_CHAIN_CHILD_LEAST];
}

void good_tree_unlink(good_tree_t *self)
{
    good_tree_t *root = NULL;

    if (!self)
        return;

    /*
     * 获取父节点
     */
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

    /*
     * NODE 是首?
    */
    if (self == root->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
    {
        root->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];
        if (root->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
            root->chain[GOOD_TREE_CHAIN_CHILD_FIRST]->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;
    }

    /*
     * NODE 是尾?
    */
    if (self == root->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
    {
        root->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
        if (root->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
            root->chain[GOOD_TREE_CHAIN_CHILD_LEAST]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    }

    /*
     * 打断与父节点的关系链，但同时保留子节点关系链。
    */
    self->chain[GOOD_TREE_CHAIN_FATHER] = NULL;
    self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    self->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;
}

void good_tree_insert(good_tree_t *father, good_tree_t *child, good_tree_t *where)
{
    if (!father || !child)
        return;

    /* 
     * 必须是根节点，或独立节点。
    */
    assert(NULL == child->chain[GOOD_TREE_CHAIN_FATHER]);
    assert(NULL == child->chain[GOOD_TREE_CHAIN_SIBLING_PREV]);
    assert(NULL == child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]);

    /* 
     * 绑定新父节点，并且打断旧的关系链。
    */
    child->chain[GOOD_TREE_CHAIN_FATHER] = father;
    child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    child->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;

    if (where)
    {
        assert(father = where->chain[GOOD_TREE_CHAIN_FATHER]);

        if (where == father->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
        {
            /*
             * 添加到头节点之前。
            */
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = where;

            /* 
             * 新的头节点。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = child;
        }
        else
        {
            /*
             * 添加到节点之前
            */
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
            /*
             * 添加到尾节点之后。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = father->chain[GOOD_TREE_CHAIN_CHILD_LEAST];

            /* 
             * 新的尾节点。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = child;
        }
        else
        {
            /*
             * 空链表，添加第一个节点。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = father->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = child;
        }
    }
}

void good_tree_insert2(good_tree_t *father, good_tree_t *child,int first)
{
    good_tree_t* where = NULL;

    if (!father || !child)
        return;

    if(first)
        where = good_tree_child(father,1);

    good_tree_insert(father,child,where);
}


void good_tree_clear(good_tree_t *root,void (*free_cb)(good_tree_t *node, void *opaque), void *opaque)
{
    good_tree_t *root_p = NULL;
    good_tree_t *father = NULL;
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;

    if(!root)
        return;

    /*
     * 暂时断开父子关系链，以防清理到父节点。 
    */
    father = good_tree_father(root);
    root->chain[GOOD_TREE_CHAIN_FATHER] = NULL;

    /*
     * 复制一下
    */
    root_p = root;

    while (root_p)
    {
        node = good_tree_child(root_p,0);

        if (node)
        {
            child = good_tree_child(node,0);

            /*
             * 检测是否有子节点，如果有先清理子节点。
            */
            if (child)
            {
                root_p = node;
            }
            else
            {
                good_tree_unlink(node);

                if(free_cb)
                {
                    free_cb(node,opaque);
                    node = NULL;
                }
                else 
                {
                    good_tree_free(&node);
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
     * 恢复关系链，关联到父节点。 
    */
    root->chain[GOOD_TREE_CHAIN_FATHER] = father;
}

void good_tree_clear2(good_tree_t *root)
{
    good_tree_clear(root,NULL,NULL);
}

void good_tree_free(good_tree_t **root)
{
    good_tree_t *root_p = NULL;

    if(!root ||!*root)
        return ;

    root_p = *root;

    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_FATHER]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_SIBLING_PREV]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_CHILD_FIRST]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_CHILD_LEAST]);

    good_buffer_unref((void**)root);
}

good_tree_t *good_tree_alloc(size_t size)
{
    good_tree_t *node = (good_tree_t *)good_buffer_alloc2(sizeof(good_tree_t) + size);

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

void good_tree_traversal(const good_tree_t *root, good_tree_iterator *it)
{
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;
    size_t deep = 1;// begin 1

    if (!root || !it || !it->dump_cb)
        return;

    it->dump_cb(0,root,it->opaque);

    it->stack[0] = NULL;
    node = good_tree_child(root,1);

    while(node)
    {
        it->dump_cb(deep,node,it->opaque);

        child = good_tree_child(node,1);

        if(child)
        {
            assert(it->stack_size > deep);

            it->stack[deep++] = node;

            node = child;
        }
        else
        {
            node = good_tree_sibling(node,0);

            while (!node && deep > 1)
            {
                node = it->stack[--deep];
                node = good_tree_sibling(node,0);
            }
        }
    }
}

void good_tree_fprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,...)
{
    va_list vaptr;
    va_start(vaptr, fmt);

    good_tree_vfprintf(fp,deep,node,fmt,vaptr);

    va_end(vaptr);
}

void good_tree_vfprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,va_list args)
{
    if (deep <= 0)
    {
        vfprintf(fp,fmt,args);
    }
    else
    {
        for (size_t i = 0; i < deep - 1; i++)
        {
            if ((i + 1 == deep - 1) && !good_tree_sibling(good_tree_father(node),0))
            {
                fprintf(fp,"    ");
            }
            else
            {
                fprintf(fp,"   │");
            }
        }

        if(good_tree_sibling(node,0))
            fprintf(fp,"   ├── ");
        else 
            fprintf(fp,"   └── ");

        vfprintf(fp,fmt,args);
    }
}