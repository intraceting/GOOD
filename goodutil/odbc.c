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

SQLRETURN good_odbc_disconnect(good_odbc_t *ctx)
{
    SQLRETURN chk;

    assert(ctx != NULL);

    /**/
    good_allocator_unref(&ctx->stat);

    if(ctx->stmt)
    {
        chk = SQLFreeHandle(SQL_HANDLE_STMT, ctx->stmt);
        if (_good_odbc_check_return(chk) != SQL_SUCCESS)
            goto final_error;
        
        ctx->stmt = NULL;
    }

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

SQLRETURN good_odbc_connect(good_odbc_t *ctx,const char * uri)
{
    SQLRETURN chk;

    assert(ctx != NULL && uri != NULL);

    chk = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &ctx->env);
    if (_good_odbc_check_return(chk) != SQL_SUCCESS)
        goto final_error;

    chk = SQLSetEnvAttr(ctx->env, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
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

#endif //__SQLEXT_H
