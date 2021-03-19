/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_BLOOM_H
#define GOOD_UTIL_BLOOM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>

#include "general.h"


int good_bloom_mark(uint8_t* pool,size_t size,size_t number);

int good_bloom_unset(uint8_t* pool,size_t size,size_t number);


#endif //GOOD_UTIL_BLOOM_H