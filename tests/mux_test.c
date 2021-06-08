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
#include "goodutil/general.h"
#include "goodutil/getargs.h"
#include "goodswitch/mux.h"

void test_server(good_tree_t *t)
{
    good_mux_t *m = good_mux_alloc();

    assert(good_mux_attach(m, 10000, 100) == 0);
    //   assert(good_mux_attach(m,10000,100)==0);

    assert(good_mux_detach(m, 10000) == 0);

    good_mux_free(&m);
}

void test_client(good_tree_t *t)
{
    
}

int main(int argc, char **argv)
{
    good_tree_t *t = good_tree_alloc(NULL);

    good_getargs(t,argc,argv,"--");

    if(good_option_exist(t,"--server"))
        test_server(t);
    else 
        test_client(t);

    good_tree_free(&t);

    return 0;
}