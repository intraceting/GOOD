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
#include "libutil/dirent.h"
#include "libutil/file.h"
#include "libutil/string.h"


int main(int argc, char **argv)
{
    char* path = good_heap_alloc(PATH_MAX);
    char* path2 = good_heap_alloc(PATH_MAX);
    char* path3 = good_heap_alloc(PATH_MAX);
    char* path4 = good_heap_alloc(PATH_MAX);

    good_dircat(path,"/tmp/abcd/efg/heigh//dfgsdfg///../..////////aaa.txt");

  //  good_mkdir(path,0);

    good_dirname(path2,path);

    good_basename(path3,path);

    printf("%s : %s\n",path2,path3);

    good_heap_free(path);
    good_heap_free(path2);
    good_heap_free(path3);

    strcpy(path4,"              asdfasdfsad     \r\n\t");

    printf("[%s]\n",path4);

    good_strtrim(path4,iscntrl,2);
    good_strtrim(path4,isblank,2);

    printf("[%s]\n",path4);



    good_heap_free(path4);


    return 0;
}