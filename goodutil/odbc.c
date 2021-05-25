/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "odbc.h"

#ifdef __SQLEXT_H

SQLRETURN _good_odbc_check_return(SQLRETURN ret)
{
    SQLRETURN chk = SQL_ERROR;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
        chk = SQL_SUCCESS;
    else
        chk = ret;

    return chk;
}

SQLRETURN good_odbc_clear_stmt(good_odbc_t *ctx)
{
    SQLRETURN chk;

    assert(ctx != NULL);

    if (ctx->attr)
    {
        for (size_t i = 0; i < ctx->attr->numbers; i++)
            good_allocator_unref((good_allocator_t**)&ctx->attr->pptrs[i]);

        good_allocator_unref(&ctx->attr);
    }

    if (ctx->stmt)
    {
        chk = SQLFreeHandle(SQL_HANDLE_STMT, ctx->stmt);
        if (_good_odbc_check_return(chk) != SQL_SUCCESS)
            goto final_error;

        ctx->stmt = NULL;
    }

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_disconnect(good_odbc_t *ctx)
{
    SQLRETURN chk;

    assert(ctx != NULL);

    chk = good_odbc_clear_stmt(ctx);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    if (ctx->dbc)
    {
        chk = SQLDisconnect(ctx->dbc);
        if (_good_odbc_check_return(chk) != SQL_SUCCESS)
            goto final_error;

        chk = SQLFreeHandle(SQL_HANDLE_DBC, ctx->dbc);
        if (_good_odbc_check_return(chk) != SQL_SUCCESS)
            goto final_error;

        ctx->dbc = NULL;
    }

    if (ctx->env)
    {
        chk = SQLFreeHandle(SQL_HANDLE_ENV, ctx->env);
        if (_good_odbc_check_return(chk) != SQL_SUCCESS)
            goto final_error;

        ctx->env = NULL;
    }

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_connect(good_odbc_t *ctx, const char *uri)
{
    SQLRETURN chk;

    assert(ctx != NULL && uri != NULL);

    chk = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &ctx->env);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = SQLSetEnvAttr(ctx->env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = SQLAllocHandle(SQL_HANDLE_DBC, ctx->env, &ctx->dbc);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = SQLDriverConnect(ctx->dbc, (SQLHWND)NULL, (SQLCHAR *)uri, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_autocommit(good_odbc_t *ctx, int enable)
{
    long flag;
    SQLRETURN chk;

    assert(ctx != NULL);

    flag = (enable ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);

    chk = SQLSetConnectAttr(ctx->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)flag, 0);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_tran_end(good_odbc_t *ctx, SQLSMALLINT type)
{
    SQLRETURN chk;

    assert(ctx != NULL);

    chk = SQLEndTran(SQL_HANDLE_DBC, ctx->dbc, type);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLSMALLINT good_odbc_name2index(good_odbc_t *ctx, const char *name)
{
    SQLSMALLINT columns;
    good_allocator_t *p;
    SQLSMALLINT index;
    SQLRETURN chk;

    assert(ctx != NULL && name != NULL);

    /*可能要先获取数据集属性。*/
    if (!ctx->attr)
    {
        chk = SQLNumResultCols(ctx->stmt, &columns);
        if (_good_odbc_check_return(chk) != SQL_SUCCESS)
            goto final_error;

        ctx->attr = good_allocator_alloc(NULL, columns, 0);
        if (!ctx->attr)
            goto final_error;

        size_t sizes[6] = {NAME_MAX, sizeof(SQLSMALLINT), sizeof(SQLSMALLINT), sizeof(SQLULEN), sizeof(SQLSMALLINT), sizeof(SQLSMALLINT)};

        for (size_t i = 0; i < ctx->attr->numbers; i++)
        {
            p = good_allocator_alloc(sizes, GOOD_ARRAY_SIZE(sizes), 0);
            if(!p)
                goto final_error;

            ctx->attr->pptrs[i] = (uint8_t*)p;

            chk = SQLDescribeCol(ctx->stmt, (SQLSMALLINT)(i + 1), p->pptrs[0], p->sizes[0], (SQLSMALLINT*)p->pptrs[1],
                                 (SQLSMALLINT*)p->pptrs[2], (SQLULEN*)p->pptrs[3], (SQLSMALLINT*)p->pptrs[4], (SQLSMALLINT*)p->pptrs[5]);

            if (_good_odbc_check_return(chk) != SQL_SUCCESS)
                goto final_error;
        }

        /*回调自己。*/
        index = good_odbc_name2index(ctx,name);
    }
    else
    {
        for (index = ctx->attr->numbers - 1; index >= 0; index--)
        {
            p = (good_allocator_t *)ctx->attr->pptrs[index];
            if (!p)
                goto final_error;

            if (good_strcmp((char *)p->pptrs[0], name, 0) == 0)
                break;
        }
    }

    return index;

final_error:

    for (size_t i = 0; i < ctx->attr->numbers; i++)
        good_allocator_unref((good_allocator_t**)&ctx->attr->pptrs[i]);

    good_allocator_unref(&ctx->attr);

    return -1;
}

SQLRETURN good_odbc_prepare(good_odbc_t *ctx, const char *sql)
{
    SQLRETURN chk;

    assert(ctx != NULL && sql != NULL);

    /*清理旧的数据集。*/
    good_odbc_clear_stmt(ctx);

    chk = SQLAllocHandle(SQL_HANDLE_STMT, ctx->dbc, &ctx->stmt);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = SQLSetStmtAttr(ctx->stmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_STATIC, 0);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = SQLPrepare(ctx->stmt, (SQLCHAR *)sql, SQL_NTS);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_execute(good_odbc_t *ctx)
{
    SQLRETURN chk;

    assert(ctx != NULL);

    chk = SQLExecute(ctx->stmt);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_exec_direct(good_odbc_t *ctx,const char *sql)
{
    SQLRETURN chk;

    chk = good_odbc_prepare(ctx,sql);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = good_odbc_execute(ctx);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_affect(good_odbc_t *ctx, SQLLEN *rows)
{
    SQLRETURN chk;

    assert(ctx != NULL && rows != NULL);

    chk = SQLRowCount(ctx->stmt, rows);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

SQLRETURN good_odbc_fetch(good_odbc_t *ctx, SQLSMALLINT direction, SQLLEN offset)
{
    SQLRETURN chk;

    assert(ctx != NULL && (direction >= 1 && direction <= 6));

    chk = SQLFetchScroll(ctx->stmt, direction, offset);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    return SQL_SUCCESS;

final_error:

    return chk;
}

#endif //__SQLEXT_H
