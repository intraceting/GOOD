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
    assert(good_odbc_autocommit(o,1) == SQL_SUCCESS);

    SQLRETURN chk = good_odbc_prepare(o,"select * from LTR_TASK_TRACE where CAST(id AS SIGNED) > ?;");

  //  SQLRETURN chk = good_odbc_prepare(o,"select * from LTR_TASK_TRACE;");

    assert(chk == SQL_SUCCESS);

    size_t id = 1000000000000;

    chk = SQLBindParameter(o->stmt, 1, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_NUMERIC, 0, 0,
                                     (SQLPOINTER)&id, sizeof(size_t), NULL);

    assert(chk == SQL_SUCCESS);

    chk = good_odbc_execute(o);

    assert(chk == SQL_SUCCESS || chk == SQL_NO_DATA);

    chk = good_odbc_fetch_prev(o);

    assert(chk == SQL_SUCCESS || chk == SQL_NO_DATA);

    while(chk == SQL_SUCCESS)
    {
        char id[100]={0};
        char tid[100]={0};
        char sid[100]={0};
        char ctime[100]={0};

        good_odbc_get_data(o,good_odbc_name2index(o,"id"),SQL_C_CHAR,id,100,NULL);

        SQLCHAR state=0;
        SQLINTEGER native =0;
        char msg[100]={0};
        SQLError(o->env,o->dbc,o->stmt,&state,&native,msg,100,NULL);

        printf("state=%hhu,native=%d,msg=%s\n",state,native,msg);

        good_odbc_get_data(o,good_odbc_name2index(o,"tid"),SQL_C_CHAR,tid,100,NULL);
        good_odbc_get_data(o,good_odbc_name2index(o,"sid"),SQL_C_CHAR,sid,100,NULL);
        good_odbc_get_data(o,good_odbc_name2index(o,"ctime"),SQL_C_CHAR,ctime,100,NULL);

        printf("%s,%s,%s,%s\n",id,tid,sid,ctime);

        chk = good_odbc_fetch_next(o);
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