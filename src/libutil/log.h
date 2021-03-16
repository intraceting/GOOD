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
#include "thread.h"

/**
 * @brief 初始化日志环境
 * 
 * @param ident 自定义标识。
 * @param copy2stderr !0 复制输出到stderr。
 */
void good_log_init(const char *ident, int copy2stderr);

/**
 * @brief 设置日志掩码
 * 
 * @param mask 新的掩码
 * 
 * @return 旧的掩码
 */
int good_log_mask(int mask);


void good_log_printf(int level,const char* fmt,...);


#endif //GOOD_UTIL_LOG_H
