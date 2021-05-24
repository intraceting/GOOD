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
#include "goodutil/odbc.h"

#ifdef __SQLEXT_H


void test_insert(good_odbc_t *o)
{

}


int main(int argc, char **argv)
{

    good_odbc_t o = {0};

    assert(good_odbc_connect(&o,argv[1]) == SQL_SUCCESS);


    return 0;
}

#else

int main(int argc, char **argv)
{
    
 
    return 0;
}

#endif //__SQLEXT_H