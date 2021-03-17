/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "map.h"


uint64_t good_map_hash(const void* data,size_t size)
{
    return good_hash_bkdr64(data,size);
}

int good_map_compare(const void *data1, const void *data2, size_t size)
{
    return memcmp(data1,data2,size);
}

void good_map_destroy(good_map_t *map)
{
    if (!map)
        return;

    if (map->size <= 0 || !map->table)
        return;

    for (size_t i = 0; i < map->size; i++)
    {
        /*
         * 也许还未创建。
        */
        if(!map->table[i])
            continue;

        good_tree_free(&map->table[i]);
    }

    good_heap_freep((void**)&map->table);
    
    memset(map,0,sizeof(*map));
}

int good_map_init(good_map_t *map, size_t size)
{
    if (!map || size <= 0)
        return -1;

    map->table = (good_tree_t**)good_heap_alloc(size * sizeof(good_tree_t*));
    if(!map->table)
        return -1;
    
    map->size = size;

    /*
     * 如果未指定，则启用默认函数。
    */
    if(!map->hash_cb)
        map->hash_cb = good_map_hash;
    if(!map->compare_cb)
        map->compare_cb = good_map_compare;

    return 0;
}
