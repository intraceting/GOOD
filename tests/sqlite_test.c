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
#include "goodutil/sqlite.h"


void test_insert(sqlite3 *ctx)
{
    int chk = good_sqlite_exec_direct(ctx, "CREATE TABLE COMPANY("
                                 "ID INT PRIMARY KEY     NOT NULL,"
                                 "NAME           TEXT    NOT NULL,"
                                 "AGE            INT     NOT NULL,"
                                 "ADDRESS        CHAR(50)"
                                 ");");

    assert(chk==SQLITE_OK);

    chk = good_sqlite_tran_begin(ctx);
    assert(chk==SQLITE_OK);

    for(int i = 1;i<1000000;i++)
    {
        char sql[200] = {0};
        sprintf(sql,"insert into COMPANY VALUES('%d','haha',123,'ertwertert');",i);
        chk = good_sqlite_exec_direct(ctx,sql);

        assert(chk==SQLITE_OK);
    }

    chk = good_sqlite_tran_commit(ctx);
    assert(chk==SQLITE_OK);
}

void test_select(sqlite3 *ctx)
{

    sqlite3_stmt *stmt = good_sqlite_prepare(ctx,"select * from COMPANY where ID > ?");

    int chk = sqlite3_bind_int(stmt,1,10);
    assert(chk==SQLITE_OK);

    int more = good_sqlite_step(stmt);
    while (more > 0)
    {
        printf("%d,%s,%d,%s\n",
                sqlite3_column_int(stmt,good_sqlite_name2index(stmt,"ID")),
                sqlite3_column_text(stmt,good_sqlite_name2index(stmt,"NAME")),
                sqlite3_column_int(stmt,good_sqlite_name2index(stmt,"AGE")),
                sqlite3_column_text(stmt,good_sqlite_name2index(stmt,"ADDRESS")));

        more = good_sqlite_step(stmt);
    }

    good_sqlite_finalize(stmt);
}


int main(int argc, char **argv)
{
    
    sqlite3 * ctx = good_sqlite_memopen();

    test_insert(ctx);

    test_select(ctx);

    good_sqlite_close(ctx);

    return 0;
}