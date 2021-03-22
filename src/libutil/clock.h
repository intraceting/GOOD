/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_CLOCK_H
#define GOOD_UTIL_CLOCK_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "general.h"
#include "thread.h"
#include "heap.h"
#include "time.h"

/**
 * 计时器-重置
 * 
 * @note 线程内有效。
*/
void good_clock_reset();

/**
 * 计时器-打点
 * 
 * @param step !NULL(0) 填写两次打点间隔时长。NULL(0) 忽略。
 * 
 * @return 计时器启动/重置到当前打点间隔时长。
 * 
 * @note 线程内有效。
*/
uint64_t good_clock_dot(uint64_t *step);

/**
 * 计时器-打点
 * 
 * @param dot !NULL(0) 计时器启动/重置到当前打点间隔时长。NULL(0) 忽略。
 * 
 * @return 两次打点间隔时长。
 * 
 * @note 线程内有效。
*/
uint64_t good_clock_step(uint64_t *dot);

#endif //GOOD_UTIL_CLOCK_H