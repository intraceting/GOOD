/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "map.h"

uint64_t abtk_map_hash(const void *data, size_t size, void *opaque)
{
    return abtk_hash_bkdr64(data, size);
}

int abtk_map_compare(const void *data1, const void *data2, size_t size, void *opaque)
{
    return memcmp(data1, data2, size);
}

void abtk_map_destroy(abtk_map_t *map)
{
    assert(map);

    /* 全部释放。*/
    abtk_tree_free(&map->table);

    memset(map, 0, sizeof(*map));
}

int abtk_map_init(abtk_map_t *map, size_t size)
{
    assert(map && size > 0);

    /* 创建树节点，用于表格。 */
    map->table = abtk_tree_alloc2(NULL, size,0);

    if (!map->table)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, -1);

    /* 如果未指定，则启用默认函数。 */
    if (!map->hash_cb)
        map->hash_cb = abtk_map_hash;
    if (!map->compare_cb)
        map->compare_cb = abtk_map_compare;

    return 0;
}

static abtk_tree_t *_abtk_map_find(abtk_map_t *map, const void *key, size_t ksize, size_t vsize)
{
    abtk_tree_t *it = NULL;
    abtk_tree_t *node = NULL;
    uint64_t hash = 0;
    uint64_t bucket = -1;
    int chk = 0;

    assert(map && key && ksize > 0);

    assert(map->table);
    assert(map->hash_cb);
    assert(map->compare_cb);

    hash = map->hash_cb(key, ksize, map->opaque);
    bucket = hash % map->table->alloc->numbers;

    /* 查找桶，不存在则创建。*/
    it = (abtk_tree_t *)map->table->alloc->pptrs[bucket];
    if (!it)
    {
        it = abtk_tree_alloc3(sizeof(bucket));
        if (it)
        {
            /*存放桶的索引值。*/
            ABTK_PTR2OBJ(uint64_t, it->alloc->pptrs[ABTK_MAP_BUCKET], 0) = bucket;

            /* 桶加入到表格中。*/
            abtk_tree_insert2(map->table, it, 0);
            map->table->alloc->pptrs[bucket] = (uint8_t *)it;
        }
    }

    if (!it)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, NULL);

    /* 链表存储的节点，依次比较查找。*/
    node = abtk_tree_child(it, 1);
    while (node)
    {
        if (node->alloc->sizes[ABTK_MAP_KEY] == ksize)
        {
            chk = map->compare_cb(node->alloc->pptrs[ABTK_MAP_KEY], key, ksize, map->opaque);
            if (chk == 0)
                break;
        }

        node = abtk_tree_sibling(node, 0);
    }

    /*如果节点不存在并且需要创建，则添加到链表头。 */
    if (!node && vsize > 0)
    {
        size_t sizes[2] = {ksize, vsize};
        node = abtk_tree_alloc2(sizes, 2,0);

        if (!node)
            ABTK_ERRNO_AND_RETURN1(ENOMEM, NULL);

        /* 注册数据节点的析构函数。*/
        if (map->destructor_cb)
            abtk_allocator_atfree(node->alloc, map->destructor_cb, map->opaque);

        /*复制KEY。*/
        memcpy(node->alloc->pptrs[ABTK_MAP_KEY], key, ksize);

        /*也许有构造函数要处理一下。*/
        if(map->construct_cb)
            map->construct_cb(node->alloc,map->opaque);

        /* 加入到链表头。 */
        abtk_tree_insert2(it, node, 1);
    }

    return node;
}

abtk_allocator_t *abtk_map_find(abtk_map_t *map, const void *key, size_t ksize, size_t vsize)
{
    abtk_tree_t *node = _abtk_map_find(map, key, ksize, vsize);

    if (node)
        return node->alloc;

    ABTK_ERRNO_AND_RETURN1(EAGAIN, NULL);
}

void abtk_map_remove(abtk_map_t *map, const void *key, size_t ksize)
{
    abtk_tree_t *node = NULL;

    assert(map);
    assert(map->table && map->hash_cb && map->compare_cb);
    assert(key && ksize > 0);

    node = _abtk_map_find(map, key, ksize, 0);
    if (node)
    {
        abtk_tree_unlink(node);
        abtk_tree_free(&node);
    }
}

static int _abtk_map_scan_cb(size_t depth, abtk_tree_t *node, void *opaque)
{
    abtk_map_t *map = (abtk_map_t *)opaque;

    /*跳过组织结构。*/
    if (depth <= 1)
        return 1;

    return map->dump_cb(node->alloc, map->opaque);
}

void abtk_map_scan(abtk_map_t *map)
{
    assert(map != NULL);
    assert(map->dump_cb != NULL);

    abtk_tree_iterator_t it = {0,_abtk_map_scan_cb,(void*)map};

    abtk_tree_scan(map->table,&it);
}