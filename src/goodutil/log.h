/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_LOG_H
#define GOOD_UTIL_LOG_H

#include "general.h"

/**
 * 日志初始化。
 * 
 * 只能执行一次。
 * 
 * @param ident NULL(0) 进程名做为标识，!NULL(0) 自定义标识。
 * @param copy2stderr 0 仅记录，!0 复制到stderr。
 */
void good_log_init(const char *ident, int copy2stderr);

/**
 * 日志重定向。
 * 
 * @param agent_cb 
 * @param opaque 
 */
void good_log_redirect(void (*agent_cb)(void *opaque,int level,const char* fmt,va_list args),void *opaque);

/**
 * 设置需要记录的日志掩码。
 * 
 * @param mask 新的掩码。
 * 
 * @return 旧的掩码。
 */
int good_log_mask(int mask);

/**
 * 记录日志。
 * 
 * 被掩码覆盖的级别才会被记录。
 * 
 * @param level LOG_* in syslog.h
 */
void good_log_printf(int level,const char* fmt,...);

/**
 * 记录日志。
 * 
 * 被掩码覆盖的级别才会被记录。
 * 
 * @param level LOG_* in syslog.h
 */
void good_log_vprintf(int level,const char* fmt,va_list args);

#endif //GOOD_UTIL_LOG_H
