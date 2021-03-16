/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "log.h"

/**
 * @brief 日志
 * 
 */
typedef struct _good_log
{
    /*
     * 掩码
    */
    int mask;

    /**
     * 代理
    */
    void (*agent_cb)(void *opaque,int level,const char* fmt,va_list args);

    /**
     * 环境指针
    */
    void *opaque;
}good_log_t;

/*
 * 全局的日志环境
*/
static good_log_t* __good_log_ctx = NULL;

void good_log_init(const char *ident, int copy2stderr)
{
    static good_log_t log = {LOG_UPTO(LOG_INFO),NULL,NULL};

    assert(__good_log_ctx = NULL);

    openlog(ident,LOG_CONS | LOG_PID | (copy2stderr ? LOG_PERROR : 0), LOG_USER);
    setlogmask(log.mask);

    /*
     * 绑定到全局日志环境。
    */
    __good_log_ctx = &log;
}

int good_log_mask(int mask)
{
    if(!__good_log_ctx)
        return 0;
    
    return setlogmask(__good_log_ctx->mask = mask);
}