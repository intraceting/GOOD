/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_HASH_H
#define GOOD_UTIL_HASH_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "macro.h"

/**
 * BKDR32
 * 
 */
uint32_t good_hash_bkdr(const void* data,size_t size);

/**
 * BKDR64
 * 
 */
uint64_t good_hash_bkdr64(const void* data,size_t size);

#endif //GOOD_UTIL_HASH_H