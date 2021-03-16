/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_LOG_H
#define GOOD_UTIL_LOG_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <syslog.h>
#include <stdarg.h>

#include "general.h"

/**
 * @brief 初始化日志环境
 * 
 * @param ident NULL(0) 进程名做为标识，!NULL(0) 自定义标识。
 * @param copy2stderr 0 仅记录，!0 复制到stderr。
 * 
 * @see openlog()
 * @see setlogmask()
 */
void good_log_init(const char *ident, int copy2stderr);

/**
 * @brief 日志重定向
 * 
 * @param agent_cb 
 * @param opaque 
 */
void good_log_redirect(void (*agent_cb)(void *opaque,int level,const char* fmt,va_list args),void *opaque);

/**
 * @brief 设置日志掩码
 * 
 * @param mask 新的掩码
 * 
 * @return 旧的掩码
 * 
 * @see setlogmask()
 */
int good_log_mask(int mask);

/**
 * @brief 记录日志
 * 
 * @param level LOG_* in syslog.h
 * @param fmt 
 * @param ... 
 * 
 * @see good_log_vprintf()
 */
void good_log_printf(int level,const char* fmt,...);

/**
 * @brief 记录日志
 * 
 * @param level LOG_* in syslog.h
 * @param fmt 
 * @param args 
 * 
 * @see vsyslog()
 */
void good_log_vprintf(int level,const char* fmt,va_list args);

#endif //GOOD_UTIL_LOG_H
