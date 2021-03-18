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
#include "libutil/map.h"
#include "libutil/buffer.h"


int main(int argc, char **argv)
{
    good_map_t m = {NULL,NULL,NULL};
    good_map_init(&m,0);

    printf("errno=%d\n",errno);


    good_map_destroy(&m);

    return 0;
}