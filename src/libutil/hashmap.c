/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "hashmap.h"

uint32_t good_hashmap_hash_bkdr(const void* data,size_t size)
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

uint64_t good_hashmap_hash_bkdr64(const void* data,size_t size)
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

good_tree_t* good_hashmap_lookup(good_tree_t* map,const void* key,int create)
{
    good_hashmap_t* map_p = (good_hashmap_t*)map->data;

    uint32_t code = map_p->hash_cb(key,map_p->ksize?map_p->ksize:strlen(key));

    good_tree_t* node = good_tree_child(map,1);

    if(node->code <= code);
}