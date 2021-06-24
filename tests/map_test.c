/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "abtkutil/map.h"
#include "abtkutil/buffer.h"


int dump_tree(size_t deep, abtk_tree_t *node, void *opaque)
{
    if(deep==0)
        abtk_tree_fprintf(stderr,deep,node,"%s\n","map");
    else if(deep ==1)
        abtk_tree_fprintf(stderr,deep,node,"%lu\n",
            *ABTK_PTR2PTR(uint64_t, node->alloc->pptrs[ABTK_MAP_BUCKET], 0));
    else
        abtk_tree_fprintf(stderr, deep, node, "%d:%s\n",
                          *ABTK_PTR2PTR(int, node->alloc->pptrs[ABTK_MAP_KEY], 0),
                          ABTK_PTR2PTR(char, node->alloc->pptrs[ABTK_MAP_VALUE], 0));

    return 1;
}

int dump_cb(abtk_allocator_t *alloc, void *opaque)
{
    fprintf(stderr, "%d:%s\n",
                          *ABTK_PTR2PTR(int, alloc->pptrs[ABTK_MAP_KEY], 0),
                          ABTK_PTR2PTR(char, alloc->pptrs[ABTK_MAP_VALUE], 0));

    return 1;
}

int dump2_tree(size_t deep, abtk_tree_t *node, void *opaque)
{
    if(deep==0)
        abtk_tree_fprintf(stderr,deep,node,"%s\n","map");
    else if(deep ==1)
        abtk_tree_fprintf(stderr,deep,node,"%lu\n",
            *ABTK_PTR2PTR(uint64_t, node->alloc->pptrs[ABTK_MAP_BUCKET], 0));
    else
        abtk_tree_fprintf(stderr, deep, node, "%s:%s\n",
                          ABTK_PTR2PTR(char, node->alloc->pptrs[ABTK_MAP_KEY], 0),
                          ABTK_PTR2PTR(char, node->alloc->pptrs[ABTK_MAP_VALUE], 0));

    return 1;
}

int dump2_cb(abtk_allocator_t *alloc, void *opaque)
{
fprintf(stderr,"%s:%s\n",
                          ABTK_PTR2PTR(char, alloc->pptrs[ABTK_MAP_KEY], 0),
                          ABTK_PTR2PTR(char, alloc->pptrs[ABTK_MAP_VALUE], 0));

    return 1;
}

void traversal_tree(abtk_tree_t *root)
{
    printf("\n-------------------------------------\n");

    abtk_tree_iterator_t it = {0,dump_tree,NULL};

    abtk_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}

void traversal2_tree(abtk_tree_t *root)
{
    printf("\n-------------------------------------\n");

    abtk_tree_iterator_t it = {0,dump2_tree,NULL};

    abtk_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}

uint64_t map_hash(const void* data,size_t size,void *opaque)
{
    return *ABTK_PTR2PTR(int,data,0);
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
    abtk_map_t m = {NULL,map_hash,NULL,NULL,NULL};


    abtk_map_init(&m,10000);

    for (int i = 0; i < 10000; i++)
    {
        //int d = rand();
        int d = i;
        abtk_allocator_t *n = abtk_map_find(&m,&d,sizeof(d),100);
        if(!n)
            break;
        
        memset(n->pptrs[ABTK_MAP_VALUE],'A'+i%26,n->sizes[ABTK_MAP_VALUE]-1);
        
    }

    traversal_tree(m.table);

    m.dump_cb = dump_cb;
    abtk_map_scan(&m);

    for(int i = 0;i<10000;i++)
        abtk_map_remove(&m,&i,sizeof(i));

    traversal_tree(m.table);

    m.dump_cb = dump_cb;
    abtk_map_scan(&m);

    getchar();

    m.hash_cb = abtk_map_hash;

    srand(time(NULL));


    int x = ((argc>1)?atoi(argv[1]):3);

    for (int i = 0; i < x ; i++)
    {
        char buf[100] ={0};
        make_str(buf,50);

        printf("%d=%s\n",i,buf);

        abtk_allocator_t *n = abtk_map_find(&m,buf,strlen(buf)+1,100);
        if(!n)
            continue;
            
        memset(n->pptrs[ABTK_MAP_VALUE],'A'+i%26,n->sizes[ABTK_MAP_VALUE]-1);
    }

    traversal2_tree(m.table);
    
    m.dump_cb = dump2_cb;
    abtk_map_scan(&m);

    abtk_map_destroy(&m);

    return 0;
}