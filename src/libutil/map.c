/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "map.h"


uint64_t good_map_hash(const void* data,size_t size,void *opaque)
{
    return good_hash_bkdr64(data,size);
}

int good_map_compare(const void *data1, const void *data2, size_t size,void *opaque)
{
    return memcmp(data1,data2,size);
}

void good_map_destroy(good_map_t *map)
{
    assert(map);
    
    /*
     * 全部释放。
    */
    good_tree_free(&map->table);
    
    memset(map,0,sizeof(*map));
}

int good_map_init(good_map_t *map, size_t size)
{
    assert(map && size > 0);

    map->table = good_tree_alloc(NULL, size);

    if (!map->table)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,-1);

    /*
     * 如果未指定，则启用默认函数。
    */
    if(!map->hash_cb)
        map->hash_cb = good_map_hash;
    if(!map->compare_cb)
        map->compare_cb = good_map_compare;

    return 0;
}

good_tree_t* good_map_find(good_map_t* map,const void* key,size_t ksize,size_t vsize)
{
    good_tree_t *it = NULL;
    good_tree_t *node = NULL;
    uint64_t hash = 0;
    uint64_t index = -1;
    int chk = 0;

    assert(map && key && ksize > 0);

    assert(map->table);
    assert(map->hash_cb);
    assert(map->compare_cb);

    hash = map->hash_cb(key, ksize,map->opaque);
    index = hash % map->table->buf->number;

    it = (good_tree_t *)map->table->buf->data[index];
    if(!it)
    {
        it = good_tree_alloc2(sizeof(index));
        if(it)
        {
            *GOOD_PTR2PTR(uint64_t,it->buf->data[GOOD_MAP_INDEX],0) = index;
            it->buf->size1[GOOD_MAP_INDEX] = sizeof(index);

            good_tree_insert2(map->table,it,0);
            map->table->buf->data[index] = (uint8_t*)it;
        }
    }

    if(!it)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    node = good_tree_child(it,1);

    while(node)
    {
        if (node->buf->size1[GOOD_MAP_KEY] == ksize)
        {
            chk = map->compare_cb(node->buf->data[GOOD_MAP_KEY], key, ksize, map->opaque);
            if (chk == 0)
                break;
        }

        node = good_tree_sibling(node,0);
    }

    if (!node && vsize > 0)
    {
        size_t size[2] = {ksize + 1, vsize + 1};
        node = good_tree_alloc(size, 2);

        if(!node)
            GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

        node->buf->free_cb = map->free_cb;
        node->buf->opaque = map->opaque;

        memcpy(node->buf->data[GOOD_MAP_KEY],key,ksize);
        node->buf->size1[GOOD_MAP_KEY] = ksize;

        good_tree_insert2(it,node,1);
    }

    return node;
}

void good_map_erase(good_map_t* map,const void* key,size_t ksize)
{
    good_tree_t *node = NULL;

    assert(map);
    assert(map->table && map->hash_cb && map->compare_cb);
    assert(key && ksize>0);
    
    node = good_map_find(map,key,ksize,0);
    if(node)
    {
        good_tree_unlink(node);
        good_tree_free(&node);
    }
}