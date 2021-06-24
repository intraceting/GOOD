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
#include "abtkutil/option.h"
#include "abtkutil/getargs.h"


int dump2(size_t deep, abtk_tree_t *node, void *opaque)
{
    if(deep==0)
        abtk_tree_fprintf(stderr,deep,node,"OPT\n");
    if(deep==1)
        abtk_tree_fprintf(stderr,deep,node,"%s\n",node->alloc->pptrs[ABTK_OPTION_KEY]);
    if(deep==2)
        abtk_tree_fprintf(stderr,deep,node,"%s\n",node->alloc->pptrs[ABTK_OPTION_VALUE]);

    return 1;
}

void traversal(abtk_tree_t *root)
{
    printf("\n-------------------------------------\n");

    abtk_tree_iterator_t it = {0,dump2,NULL};
    abtk_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}

void test1(int argc, char **argv)
{
    abtk_tree_t *t = abtk_tree_alloc(NULL);

    abtk_option_set(t,"-","bbb");
    abtk_option_set(t,"-","ccc");
    abtk_option_set(t,"-","fff");
    abtk_option_set(t,"-","eee");
    abtk_option_set(t,"-","www");

    assert(abtk_option_count(t,"-")==5);

    abtk_option_set(t,"-bbb","123");
    abtk_option_set(t,"-bbb","456");
    abtk_option_set(t,"-bbb","789");
    abtk_option_set(t,"-bbb","543");
    abtk_option_set(t,"-bbb","854");

    assert(abtk_option_count(t,"-bbb")==5);

    abtk_option_set(t,"-ddd",NULL);

    assert(abtk_option_exist(t,"-ddd"));

    assert(!abtk_option_exist(t,"-ccc"));

    traversal(t);

    const char* p = abtk_option_get(t,"-",0,NULL);

    printf("p=%s\n",p);

    const char* p1 = abtk_option_get(t,"-bbb",1,NULL);

    printf("p1=%s\n",p1);

    const char* p2 = abtk_option_get(t,"-ccc",1,NULL);

    assert(p2==NULL);

    p2 = abtk_option_get(t,"-ccc",1,"f");

    assert(p2[0]=='f');

    int s = abtk_option_fprintf(stderr,t);

    char buf[100] = {0};

    abtk_option_snprintf(buf,100,t);


    abtk_getargs(t,argc,argv,"--");

    printf("\n--------------------------------------\n");
    abtk_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");
    
    abtk_getargs_file(t,abtk_option_get(t,"--test-import",0,NULL),'\n','#',"test-import","--");


    printf("\n--------------------------------------\n");
    abtk_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");

    abtk_option_remove(t,"-bbb");


    printf("\n--------------------------------------\n");
    abtk_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");


    abtk_tree_free(&t);
 
}

void test2(int argc, char **argv)
{
    abtk_tree_t *t = abtk_tree_alloc(NULL);

    abtk_getargs_file(t,"/etc/os-release",'\n',0,NULL,NULL);

   printf("\n--------------------------------------\n");
    abtk_option_fprintf(stderr,t);
    printf("\n--------------------------------------\n");

    abtk_tree_free(&t);
}

int main(int argc, char **argv)
{

    test1(argc,argv);

    test2(argc,argv);




    return 0;
}