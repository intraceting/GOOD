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
#include "goodutil/option.h"
#include "goodutil/getargs.h"


int dump2(size_t deep, good_tree_t *node, void *opaque)
{
    if(deep==0)
        good_tree_fprintf(stderr,deep,node,"OPT\n");
    if(deep==1)
        good_tree_fprintf(stderr,deep,node,"%s\n",node->alloc->pptrs[GOOD_OPTION_KEY]);
    if(deep==2)
        good_tree_fprintf(stderr,deep,node,"%s\n",node->alloc->pptrs[GOOD_OPTION_VALUE]);

    return 1;
}

void traversal(good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_iterator_t it = {0,dump2,NULL};
    good_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}

int main(int argc, char **argv)
{
    good_tree_t *t = good_tree_alloc(NULL);


    good_option_set(t,"-","bbb");
    good_option_set(t,"-","ccc");
    good_option_set(t,"-","fff");
    good_option_set(t,"-","eee");
    good_option_set(t,"-","www");

    assert(good_option_count(t,"-")==5);

    good_option_set(t,"-bbb","123");
    good_option_set(t,"-bbb","456");
    good_option_set(t,"-bbb","789");
    good_option_set(t,"-bbb","543");
    good_option_set(t,"-bbb","854");

    assert(good_option_count(t,"-bbb")==5);

    good_option_set(t,"-ddd",NULL);

    assert(good_option_exist(t,"-ddd"));

    assert(!good_option_exist(t,"-ccc"));

    traversal(t);

    const char* p = good_option_get(t,"-",0,NULL);

    printf("p=%s\n",p);

    const char* p1 = good_option_get(t,"-bbb",1,NULL);

    printf("p1=%s\n",p1);

    const char* p2 = good_option_get(t,"-ccc",1,NULL);

    assert(p2==NULL);

    p2 = good_option_get(t,"-ccc",1,"f");

    assert(p2[0]=='f');

    int s = good_option_fprintf(stderr,t);

    char buf[100] = {0};

    good_option_snprintf(buf,100,t);


    good_getargs(t,argc,argv,"--");

    printf("\n--------------------------------------\n");
    good_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");
    
    good_getargs_file(t,good_option_get(t,"--test-import",0,NULL),'\n','#',"test-import","--");


    printf("\n--------------------------------------\n");
    good_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");

    good_option_remove(t,"-bbb");


    printf("\n--------------------------------------\n");
    good_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");

 

    good_tree_free(&t);

    return 0;
}