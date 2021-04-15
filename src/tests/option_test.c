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

    const good_option_value_t *v = good_option_get(&t,"aaa");

    v = good_option_get(&t,"bbb");

    good_option_destroy(&t);


    

    return 0;
}