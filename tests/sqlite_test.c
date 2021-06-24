/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "abtkutil/sqlite.h"

#ifdef _SQLITE3_H_

void test_insert(sqlite3 *ctx)
{
    int chk = abtk_sqlite_exec_direct(ctx, "CREATE TABLE COMPANY("
                                 "ID INT PRIMARY KEY     NOT NULL,"
                                 "NAME           TEXT    NOT NULL,"
                                 "AGE            INT     NOT NULL,"
                                 "ADDRESS        CHAR(50)"
                                 ");");

    assert(chk==SQLITE_OK);

    chk = abtk_sqlite_tran_begin(ctx);
    assert(chk==SQLITE_OK);

    for(int i = 1;i<1000000;i++)
    {
        char sql[200] = {0};
        sprintf(sql,"insert into COMPANY VALUES('%d','haha',123,'ertwertert');",i);
        int more = abtk_sqlite_exec_direct(ctx,sql);
        assert(more>=0);
    }

    chk = abtk_sqlite_tran_commit(ctx);
    assert(chk==SQLITE_OK);
}

void test_select(sqlite3 *ctx)
{

    sqlite3_stmt *stmt = abtk_sqlite_prepare(ctx,"select * from COMPANY where ID > ?");

    int chk = sqlite3_bind_int(stmt,1,10);
    assert(chk==SQLITE_OK);

    int more = abtk_sqlite_step(stmt);
    while (more > 0)
    {
        printf("%d,%s,%d,%s\n",
                sqlite3_column_int(stmt,abtk_sqlite_name2index(stmt,"ID")),
                sqlite3_column_text(stmt,abtk_sqlite_name2index(stmt,"NAME")),
                sqlite3_column_int(stmt,abtk_sqlite_name2index(stmt,"AGE")),
                sqlite3_column_text(stmt,abtk_sqlite_name2index(stmt,"ADDRESS")));

        more = abtk_sqlite_step(stmt);
    }

    abtk_sqlite_finalize(stmt);
}

void backup_progress(int remaining, int total, void *opaque)
{
    printf("\r%10d/%d",total-remaining,total);
}

void test_backup(sqlite3 *ctx)
{
    sqlite3 * ctx2 = abtk_sqlite_open("/tmp/abc.sqlite");

    abtk_sqlite_backup_param p = {0};

    p.src = ctx;
    p.src_name = "main";

    p.dst = ctx2;
    p.dst_name = "main";

    p.step = 10;
    p.sleep = 100;
    p.progress_cb = backup_progress;


    assert(abtk_sqlite_backup(&p)==SQLITE_OK);

    printf("\n");

    abtk_sqlite_close(ctx2);
}


int main(int argc, char **argv)
{
    
    sqlite3 * ctx = abtk_sqlite_memopen();

    test_insert(ctx);

    test_select(ctx);

    test_backup(ctx);

    abtk_sqlite_close(ctx);

    return 0;
}

#else

int main(int argc, char **argv)
{
    
 
    return 0;
}

#endif //_SQLITE3_H_