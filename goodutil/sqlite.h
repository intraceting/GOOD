/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_SQLITE_H
#define GOODUTIL_SQLITE_H

#include "general.h"

#ifdef _SQLITE3_H_

/**
 * 字段类型。
 * 
*/
enum _good_sqlite_column_type
{
    GOOD_SQLITE_COLUMN_INT = 1,
#define GOOD_SQLITE_COLUMN_INT GOOD_SQLITE_COLUMN_INT

    GOOD_SQLITE_COLUMN_INT64 = 2,
#define GOOD_SQLITE_COLUMN_INT64 GOOD_SQLITE_COLUMN_INT64

    GOOD_SQLITE_COLUMN_DOUBLE = 3,
#define GOOD_SQLITE_COLUMN_DOUBLE GOOD_SQLITE_COLUMN_DOUBLE

    GOOD_SQLITE_COLUMN_VARCHAR = 4,
#define GOOD_SQLITE_COLUMN_VARCHAR GOOD_SQLITE_COLUMN_VARCHAR

    GOOD_SQLITE_COLUMN_BLOB = 5
#define GOOD_SQLITE_COLUMN_BLOB GOOD_SQLITE_COLUMN_BLOB

};

/**
 * 日志模式。
*/
enum _good_sqlite_journal_mode
{
    GOOD_SQLITE_JOURNAL_OFF = 0,
#define GOOD_SQLITE_JOURNAL_OFF GOOD_SQLITE_JOURNAL_OFF

    GOOD_SQLITE_JOURNAL_DELETE = 1,
#define GOOD_SQLITE_JOURNAL_DELETE GOOD_SQLITE_JOURNAL_DELETE

    GOOD_SQLITE_JOURNAL_TRUNCATE = 2,
#define GOOD_SQLITE_JOURNAL_TRUNCATE GOOD_SQLITE_JOURNAL_TRUNCATE

    GOOD_SQLITE_JOURNAL_PERSIST = 3,
#define GOOD_SQLITE_JOURNAL_PERSIST GOOD_SQLITE_JOURNAL_PERSIST

    GOOD_SQLITE_JOURNAL_MEMORY = 4,
#define GOOD_SQLITE_JOURNAL_MEMORY GOOD_SQLITE_JOURNAL_MEMORY

    GOOD_SQLITE_JOURNAL_WAL = 5
#define GOOD_SQLITE_JOURNAL_WAL GOOD_SQLITE_JOURNAL_WAL
};

/**
 * 备份参数
*/
typedef struct _good_sqlite_backup_param
{
    /**目标库的指针。*/
    sqlite3 *dst;

    /**目标库的名字的指针。*/
    const char *dst_name;

    /**源库的指针。*/
    sqlite3 *src;

    /**源库的名字的指针。*/
    const char *src_name;

    /**备份步长(页数量)。*/
    int step;

    /*忙时休息时长(毫秒)。*/
    int sleep;

    /**
     * 进度函数。
     * 
     * @param remaining  剩余页数量。
     * @param total 总页数量。
     * @param opaque 环境指针。
    */
    void (*progress_cb)(int remaining, int total, void *opaque);

    /**环境指针。*/
    void *opaque;

} good_sqlite_backup_param;

/**
 * 在线备份
 * 
 * @return SQLITE_DONE(101) 成功，!SQLITE_DONE(101) 失败。
 * 
 */
int good_sqlite_backup(good_sqlite_backup_param *param);

/**
 * 关闭数据库句柄。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
*/
int good_sqlite_close(sqlite3 *ctx);

/**
 * 忙碌处理函数。
*/
int good_sqlite_busy_melt(void *opaque, int count);

/**
 * 打开数据库文件。
 * 
 * @param name 数据库文件名的指针。
 * 
 * @return !NULL(0) 成功(句柄)，NULL(0) 失败。
*/
sqlite3 *good_sqlite_open(const char *name);

/**
 * 打开内存数据库。
*/
#define good_sqlite_memopen() good_sqlite_open(":memory:")

/**
 * 执行SQL语句。
 * 
 * @warning 查询不能用。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
*/
int good_sqlite_exec(sqlite3 *ctx, const char *sql);

/**
 * 启动事物。
*/
#define good_sqlite_tran_begin(ctx) good_sqlite_exec(ctx, "begin;")

/**
 * 提交事物。
*/
#define good_sqlite_tran_commit(ctx) good_sqlite_exec(ctx, "commit;")

/**
 * 回滚事物。
*/
#define good_sqlite_tran_rollback(ctx) good_sqlite_exec(ctx, "rollback;")

/**
 * 回收空间。
*/
#define good_sqlite_tran_vacuum(ctx) good_sqlite_exec(ctx, "vacuum;")

/** 
 * 设置页大小。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
*/
int good_sqlite_pagesize(sqlite3 *ctx, int size);

/** 
 * 设置日志模式。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
*/
int good_sqlite_journal_mode(sqlite3 *ctx, int mode);

/**
 * 在查询结果中查找字段名的索引。
 * 
 * @return >= 0 成功(序号)，< 0 失败。
*/
int good_sqlite_name2index(sqlite3_stmt *stmt, const char *name);

/** 
 * 在查询结果中移动游标到下一行。
 * 
 * @return > 0 成功(有数据) , <= 0 失败(末尾或出错)。
*/
int itting_sqlite_next(sqlite3_stmt *stmt);

#endif //_SQLITE3_H_

#endif //GOODUTIL_SQLITE_H