/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_CLOCK_H
#define ABTKUTIL_CLOCK_H

#include "general.h"
#include "thread.h"

/**
 * 计时器-重置
 * 
 * @warning 线程内有效。
*/
void abtk_clock_reset();

/**
 * 计时器-打点
 * 
 * @warning 线程内有效。
 * 
 * @param step !NULL(0) 返回两次打点间隔时长(微秒)。NULL(0) 忽略。
 * 
 * @return 计时器启动/重置到当前打点间隔时长(微秒)。
 *
*/
uint64_t abtk_clock_dot(uint64_t *step);

/**
 * 计时器-打点
 * 
 * @warning 线程内有效。
 * 
 * @param dot !NULL(0) 返回计时器启动或重置到当前打点间隔时长(微秒)。NULL(0) 忽略。
 * 
 * @return 两次打点间隔时长(微秒)。
 * 
*/
uint64_t abtk_clock_step(uint64_t *dot);

#endif //ABTKUTIL_CLOCK_H