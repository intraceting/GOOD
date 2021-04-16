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
#include "libutil/option.h"

int main(int argc, char **argv)
{
    good_option_t t = {0};

    good_option_init(&t);

    good_option_set(&t,"aaa","bbb");

    good_option_set(&t,"aaa","ccc");
    good_option_set(&t,"aaa","ccc");
    good_option_set(&t,"aaa","ccc");
    good_option_set(&t,"aaa","ccc");

    good_option_set(&t,"bbb","ccc");
    good_option_set(&t,"bbb","ccc");
    good_option_set(&t,"bbb","ccc");
    good_option_set(&t,"bbb","ccc");
    good_option_set(&t,"bbb","ccc");

    const good_vector_t *v = good_option_get(&t,"aaa");

    for(size_t i = 0;i<v->count;i++)
        printf("%s\n",*((char**)good_vector_at(v,i)));

    const good_vector_t *v2 = good_option_get(&t,"bbb");

    for(size_t i = 0;i<v2->count;i++)
        printf("%s\n",*((char**)good_vector_at(v2,i)));

    good_option_destroy(&t);

    return 0;
}