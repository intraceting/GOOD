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

void good_clock_reset();

uint64_t good_clock_dot(uint64_t *step);

uint64_t good_clock_step(uint64_t *dot);

#endif //GOOD_UTIL_CLOCK_H