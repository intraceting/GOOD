/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "map.h"

uint32_t good_map_hash_bkdr(const void* data,size_t size)
{
    uint32_t seed = 131; /* 31 131 1313 13131 131313 etc.. */
    uint32_t hash = 0;
    /**/
    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + (*GOOD_PTR2PTR(uint8_t,data,i));
    }

    return hash;
}

uint64_t good_map_hash_bkdr64(const void* data,size_t size)
{
    uint64_t seed = 13113131; /* 31 131 1313 13131 131313 etc.. */
    uint64_t hash = 0;
    /**/
    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + (*GOOD_PTR2PTR(uint8_t,data,i));
    }

    return hash; 
}

int good_map_compare(const void *data1, const void *data2, size_t size)
{
    return memcmp(data1,data2,size);
}

good_map_node* good_map_lookup(good_map_t* map,const void* key,size_t ksize,int create)
{
    good_tree_t* tree_p;
    good_map_node* node;
    uint32_t hash_code;

    if (!map || !key || ksize == 0)
        return NULL;

    if(!map->hash_cb)
        map->hash_cb = good_map_hash_bkdr;
    if(!map->compare_cb)
        map->compare_cb = good_map_compare;

    tree_p = &map->root;
    hash_code = map->hash_cb(key,ksize);
    
    node = (good_map_node*)good_tree_child(tree_p,1);

    
}