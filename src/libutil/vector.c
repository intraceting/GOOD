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

void good_vector_destroy(good_vector_t *vec)
{
    if (!vec)
        return;

    if (vec->size <= 0 || !vec->table)
        return;

    for (size_t i = 0; i < vec->size; i++)
    {
        /*
         * 也许还未创建。
        */
        if(!vec->table[i])
            continue;

        good_tree_free(&vec->table[i]);
    }

    good_heap_freep((void**)&vec->table);
    
    memset(vec,0,sizeof(*vec));
}

int good_vector_init(good_vector_t *vec, size_t size)
{
    if (!vec || size <= 0)
        return -1;

    vec->table = (good_tree_t**)good_heap_alloc(size * sizeof(good_tree_t*));
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

good_tree_t* good_vector_find(good_vector_t* vec,const void* key,size_t ksize,int create)
{
    uint64_t hash;
    uint64_t index;
    good_tree_t* bucket;
    good_tree_t* value;
    good_tree_t* it;
    int chk_cmp;

    if (!vec)
        return NULL;

    assert(key != NULL);

    /*
     * Fix KEY size
    */
    if (ksize <= 0)
        ksize = strlen(key);

    hash = vec->hash_cb(key,ksize);
    index = hash % vec->size;

    bucket = good_vector_seek(vec,index);
    it = good_tree_child(bucket,1);

    while(it)
    {
        if (ksize != it->buf->size)
        {
            it = good_tree_sibling(it, 0);
        }
        else
        {
            chk_cmp = vec->compare_cb(it->buf->data, key, ksize);

            if (chk_cmp == 0)
                break;
            else
                it = good_tree_sibling(it, 0);
        }
    }

    if(!it && create)
    {
        it = good_tree_alloc(ksize);
        if(it)
        {
            memcpy(it->buf->data,key,ksize);

            good_tree_insert2(bucket,it,0);
        }
    }

    if(it)
    {
        value = good_tree_child(it, 1);
        if(!value)
        {
            value = good_tree_alloc(0);
            if(value)
                good_tree_insert2(it,value,1);
        }
    }

    return value;

}
