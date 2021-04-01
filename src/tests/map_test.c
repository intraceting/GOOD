/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "libutil/map.h"
#include "libutil/buffer.h"


int dump(size_t deep, const good_tree_t *node, void *opaque)
{
    if(deep==0)
        good_tree_fprintf(stderr,deep,node,"%s\n","map");
    else if(deep ==1)
        good_tree_fprintf(stderr,deep,node,"%lu\n",
            *GOOD_PTR2PTR(uint64_t, node->alloc->pptrs[GOOD_MAP_BUCKET_DATA], 0));
    else
        good_tree_fprintf(stderr, deep, node, "%d:%s\n",
                          *GOOD_PTR2PTR(int, node->alloc->pptrs[GOOD_MAP_KEY_DATA], 0),
                          GOOD_PTR2PTR(char, node->alloc->pptrs[GOOD_MAP_VALUE_DATA], 0));

    return 1;
}

int dump2(size_t deep, const good_tree_t *node, void *opaque)
{
    if(deep==0)
        good_tree_fprintf(stderr,deep,node,"%s\n","map");
    else if(deep ==1)
        good_tree_fprintf(stderr,deep,node,"%lu\n",
            *GOOD_PTR2PTR(uint64_t, node->alloc->pptrs[GOOD_MAP_BUCKET_DATA], 0));
    else
        good_tree_fprintf(stderr, deep, node, "%s:%s\n",
                          GOOD_PTR2PTR(char, node->alloc->pptrs[GOOD_MAP_KEY_DATA], 0),
                          GOOD_PTR2PTR(char, node->alloc->pptrs[GOOD_MAP_VALUE_DATA], 0));

    return 1;
}

void traversal(const good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_scan(root,0,dump,NULL);

    printf("\n-------------------------------------\n");
}

void traversal2(const good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_scan(root,0,dump2,NULL);

    printf("\n-------------------------------------\n");
}

uint64_t map_hash(const void* data,size_t size,void *opaque)
{
    return *GOOD_PTR2PTR(int,data,0);
}

void make_str(char*buf,size_t max)
{
    static char pool[] = {"123455678890poiuytreqwqasdfghjjklmnbvcxz"};

    size_t len = abs(rand())%max+1;

    assert(len<100);

    for(size_t i = 0;i<len;i++)
    {
        int d = rand();
        d = abs(d);
        d = d % 32;

        //printf("d[%ld]=%d\n",i,d);

        buf[i] = pool[d];
    }
}

int main(int argc, char **argv)
{
    good_map_t m = {NULL,map_hash,NULL,NULL,NULL};


    good_map_init(&m,100);

    for (int i = 0; i < 100; i++)
    {
        //int d = rand();
        int d = i;
        good_tree_t *n = good_map_find(&m,&d,sizeof(d),100);
        if(!n)
            break;
        
        memset(n->alloc->pptrs[GOOD_MAP_VALUE_DATA],'A'+i%26,n->alloc->sizes[GOOD_MAP_VALUE_DATA]-1);
        
    }

    traversal(m.table);

    for(int i = 0;i<10000;i++)
        good_map_erase(&m,&i,sizeof(i));

    traversal(m.table);

    getchar();

    m.hash_cb = good_map_hash;

    srand(time(NULL));


    int x = ((argc>1)?atoi(argv[1]):3);

    for (int i = 0; i < x ; i++)
    {
        char buf[100] ={0};
        make_str(buf,50);

        printf("%d=%s\n",i,buf);

        good_tree_t *n = good_map_find(&m,buf,strlen(buf),100);
        if(!n)
            continue;
            
        memset(n->alloc->pptrs[GOOD_MAP_VALUE_DATA],'A'+i%26,n->alloc->sizes[GOOD_MAP_VALUE_DATA]-1);
    }

    traversal2(m.table);


    good_map_destroy(&m);

    return 0;
}