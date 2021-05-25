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
     * 数据集属性。
     * 
     * @note 尽量不要直接修改。
     */
    good_allocator_t *attr;

} good_odbc_t;

/**
 * 清理数据集。
*/
SQLRETURN good_odbc_clear_stmt(good_odbc_t *ctx);

/**
 * 断开连接。
*/
SQLRETURN good_odbc_disconnect(good_odbc_t *ctx);

/**
 * 连接数据库。
 * 
 * @note 默认：禁用自动提交。
*/
SQLRETURN good_odbc_connect(good_odbc_t *ctx, const char *uri);

/**
 * 启用或禁用自动提交。
 * 
 * @param enable !0 启用，0 禁用。
*/
SQLRETURN good_odbc_autocommit(good_odbc_t *ctx,int enable);

/**
 * 开启事务(关闭自动提交)。
*/
#define good_odbc_tran_begin(ctx)   good_odbc_autocommit(ctx,0)

/**
 * 结束事务。
*/
SQLRETURN good_odbc_tran_end(good_odbc_t *ctx, SQLSMALLINT type);

/**
 * 提交事务。
*/
#define good_odbc_tran_commit(ctx) good_odbc_tran_end(ctx, SQL_COMMIT)

/**
 * 回滚事务。
*/
#define good_odbc_tran_rollback(ctx) good_odbc_tran_end(ctx, SQL_ROLLBACK)

/**
 * 在数据集中查找字段的索引。
 * 
 * @return >= 0 成功(索引)，< 0 失败(未找到)。
*/
SQLSMALLINT good_odbc_name2index(good_odbc_t *ctx, const char *name);

/**
 * 准备SQL语句。
*/
SQLRETURN good_odbc_prepare(good_odbc_t *ctx, const char *sql);

/**
 * 执行SQL语句。
 * 
 * @return SQL_SUCCESS(0) 成功，SQL_NO_DATA(100) 无数据，< 0 失败。
*/
SQLRETURN good_odbc_execute(good_odbc_t *ctx);

/**
 * 直接执行SQL语句。
 * 
 * @see good_odbc_prepare
 * @see good_odbc_execute
 * 
*/
SQLRETURN good_odbc_exec_direct(good_odbc_t *ctx,const char *sql);

/**
 * 返回影响的行数。
*/
SQLRETURN good_odbc_affect(good_odbc_t *ctx,SQLLEN *rows);

/**
 * 在数据集中移动游标。
 * 
 * @return SQL_SUCCESS(0) 成功，SQL_NO_DATA(100) 无数据(游标已经在数据集的首或尾)，< 0 失败。
*/
SQLRETURN good_odbc_fetch(good_odbc_t *ctx, SQLSMALLINT direction, SQLLEN offset);

/**
 * 在数据集中向上移动游标。
*/
#define good_odbc_fetch_prev(ctx, offset) good_odbc_fetch(ctx, SQL_FETCH_PRIOR, offset)

/**
 * 在数据集中向下移动游标。
*/
#define good_odbc_fetch_next(ctx, offset) good_odbc_fetch(ctx, SQL_FETCH_NEXT, offset)

#endif //__SQLEXT_H

#endif //GOODUTIL_ODBC_H