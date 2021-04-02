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

    /*
     * 创建树节点，用于表格。
    */
    map->table = good_tree_alloc2(NULL, size);

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
    uint64_t bucket = -1;
    int chk = 0;

    assert(map && key && ksize > 0);

    assert(map->table);
    assert(map->hash_cb);
    assert(map->compare_cb);

    hash = map->hash_cb(key, ksize,map->opaque);
    bucket = hash % map->table->alloc->numbers;

    /*
     * 查找桶，不存在创建。
    */
    it = (good_tree_t *)map->table->alloc->pptrs[bucket];
    if(!it)
    {
        it = good_tree_alloc3(sizeof(bucket));
        if(it)
        {
            /*
             * 存放桶的索引值。
            */
            *GOOD_PTR2PTR(uint64_t,it->alloc->pptrs[GOOD_MAP_BUCKET],0) = bucket;

            /*
             * 桶加和入到表格中。
            */
            good_tree_insert2(map->table,it,0);
            map->table->alloc->pptrs[bucket] = (uint8_t*)it;
        }
    }

    if(!it)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    /*
     * 链表存储的节点，依次比较查找。
    */
    node = good_tree_child(it,1);
    while(node)
    {
        if (node->alloc->sizes[GOOD_MAP_KEY] == ksize)
        {
            chk = map->compare_cb(node->alloc->pptrs[GOOD_MAP_KEY], key, ksize, map->opaque);
            if (chk == 0)
                break;
        }

        node = good_tree_sibling(node,0);
    }

    /*
     * 如果节点不存在并且需要创建，则添加到链表头。
    */
    if (!node && vsize > 0)
    {
        size_t sizes[2] = {ksize,vsize};
        node = good_tree_alloc2(sizes, 2);

        if(!node)
            GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

        /*
         * 只为数据节点注册析构函数。
        */
        if(map->destroy_cb)
            good_allocator_atfree(node->alloc,map->destroy_cb,map->opaque);

        memcpy(node->alloc->pptrs[GOOD_MAP_KEY],key,ksize);

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