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
#include <limits.h>
#include "libutil/file.h"


int main(int argc, char **argv)
{
    char* path = good_heap_alloc(PATH_MAX);
    char* path2 = good_heap_alloc(PATH_MAX);
    char* path3 = good_heap_alloc(PATH_MAX);
    char* path4 = good_heap_alloc(PATH_MAX);

    good_path_concat(path,"/tmp/abcd/efg/heigh/aaa.txt");

    good_path_create(path,0);

    good_path_dirname(path2,path);

    good_path_basename(path3,path);

    printf("%s : %s\n",path2,path3);

    good_heap_free(path);
    good_heap_free(path2);
    good_heap_free(path3);



    good_heap_free(path4);


    return 0;
}