/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "vector.h"


uint64_t good_vector_default_hash(const void* data,size_t size)
{
    return good_hash_bkdr64(data,size);
}

int good_vector_default_compare(const void *data1, const void *data2, size_t size)
{
    return memcmp(data1,data2,size);
}

void good_vector_default_free(void* m)
{
    good_buffer_unref(&m);
}

void good_vector_destroy(good_vector_t *vec)
{
    if (!vec)
        return;

    if (vec->size <= 0 || !vec->table)
        return;
    
    if(!vec->free_cb)
        return;

    for (size_t i = 0; i < vec->size; i++)
    {
        good_tree_t* it = vec->table[i];

        /*
         * 也许还未创建。
        */
        if(!it)
            continue;

        /*
         * 也许有KEY-VALUE
        */
        good_tree_t* key = good_tree_child(it,1);
        while(key)
        {
            /*
             * VALUE是KEY的子节点。
            */
            good_tree_t* val = good_tree_child(key,1);
            if(val)
            {
                /**
                 * VALUE 释放要特殊处理一下。
                */
                vec->free_cb(val->data);
                val->data = NULL;
                val->size = 0;

                good_tree_unlink(val);
                good_tree_free(&val);
            }

            good_tree_unlink(key);
            good_tree_free(&key);

            /*
             * 下一个KEY。
            */
            key = good_tree_child(it,1);
        }

        /**
        * ITEM 释放要特殊处理一下。
        */
        vec->free_cb(it->data);
        it->data = NULL;
        it->size = 0;

        good_tree_free(&vec->table[i]);
    }

    good_buffer_unref((void**)&vec->table);
    memset(vec,0,sizeof(*vec));
}

int good_vector_init(good_vector_t *vec, size_t size)
{
    if (!vec || size <= 0)
        return -1;

    vec->table = (good_tree_t**)good_buffer_alloc2(size * sizeof(good_tree_t*));
    if(!vec->table)
        return -1;
    
    vec->size = size;

    /*
     * 如果指定，则启用默认函数。
    */
    if(!vec->hash_cb)
        vec->hash_cb = good_vector_default_hash;
    if(!vec->compare_cb)
        vec->compare_cb = good_vector_default_compare;
    if(!vec->free_cb)
        vec->free_cb = good_vector_default_free;

    return 0;
}

good_tree_t* good_vector_seek(good_vector_t* vec,size_t index)
{
    if (!vec)
        return NULL;
    
    assert(index < vec->size);

    if(!vec->table[index])
        vec->table[index] = good_tree_alloc(0);

    return vec->table[index];
}