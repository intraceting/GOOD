/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_TIME_H
#define GOOD_UTIL_TIME_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "general.h"

/**
 * 时间戳整形化
 * 
 * @param precision 精度。0～9。
 * 
 * @note 当精度为纳秒时，公元2444年之前或者时长544年之内有效。
*/
uint64_t good_time_clock2kind(struct timespec* ts,uint8_t precision);

/**
 * 获取时间戳并整形化
 * 
 * @param id CLOCK_* in time.h
 * 
 * @see clock_gettime()
 * @see good_time_clock2kind()
*/
uint64_t good_time_clock2kind_with(clockid_t id ,uint8_t precision);

/**
 * 本地时间转国际时间
 * 
 * @param reverse 0 本地转国际，!0 国际转本地。
*/
struct tm* good_time_local2utc(struct tm* dst,const struct tm* src,int reverse);

/**
 * 获取自然时间
 * 
 * @param reverse 0 本地，!0 国际。
*/
struct tm* good_time_get(struct tm* tm,int utc);

#endif //GOOD_UTIL_TIME_H