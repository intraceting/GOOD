/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_ODBC_H
#define GOODUTIL_ODBC_H

#include "general.h"
#include "allocator.h"

#ifdef __SQLEXT_H

/**
 * ODBC接口。
*/
typedef struct _good_odbc
{
    /** 环境。 */
    SQLHENV env;

    /** 连接。 */
    SQLHDBC dbc;

    /** 数据集。 */
    SQLHSTMT stmt;

    /** 
     * 数据集状态。
     * 
     * @note 尽量不要直接修改。
     */
    good_allocator_t *stat;

} good_odbc_t;

/**
 * 
*/
SQLRETURN good_odbc_disconnect(good_odbc_t *ctx);

/**
 * 连接数据库。
*/
SQLRETURN good_odbc_connect(good_odbc_t *ctx, const char *uri);

#endif //__SQLEXT_H

#endif //GOODUTIL_ODBC_H