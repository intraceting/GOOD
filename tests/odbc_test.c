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
    assert(good_odbc_tran_begin(o) == SQL_SUCCESS);

    time_t begin = time(NULL);

    for(time_t i = begin+1;i<begin+1000;i++)
    {
        char sql[200] = {0};
        sprintf(sql,"insert into LTR_TASK_TRACE(ID,TID,SID) VALUES('%ld','haha','123');",i);
        SQLRETURN chk = good_odbc_exec_direct(o,sql);

        assert(chk==SQL_SUCCESS);
    }

    assert(good_odbc_tran_commit(o) == SQL_SUCCESS);
}

void test_select(good_odbc_t *o)
{
    SQLRETURN chk = good_odbc_prepare(o,"select * from LTR_TASK_TRACE where CAST(id AS SIGNED) > ?;");

    assert(chk == SQL_SUCCESS);

    size_t id = 100;

    chk = SQLBindParameter(o->stmt, 1, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_NUMERIC, 0, 0,
                                     (SQLPOINTER)&id, sizeof(size_t), NULL);

    assert(chk == SQL_SUCCESS);

    chk = good_odbc_execute(o);

    assert(chk == SQL_SUCCESS || chk == SQL_NO_DATA);

    while(chk == SQL_SUCCESS)
    {
        

        chk = good_odbc_fetch_next(o,0);
    }

}


int main(int argc, char **argv)
{

    good_odbc_t o = {0};

    assert(good_odbc_connect(&o,argv[1]) == SQL_SUCCESS);

    test_insert(&o);

    test_select(&o);


    assert(good_odbc_disconnect(&o) == SQL_SUCCESS);

    return 0;
}

#else

int main(int argc, char **argv)
{
    
 
    return 0;
}

#endif //__SQLEXT_H