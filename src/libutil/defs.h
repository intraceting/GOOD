/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_DEFS_H
#define GOOD_UTIL_DEFS_H

/**/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE

/**/
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include <pthread.h>
#include <ctype.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <libgen.h>
#include <fnmatch.h>
#include <limits.h>
#include <dirent.h>
#include <poll.h>
#include <iconv.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <tar.h>
#include <termios.h>

#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

#include <sys/socket.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/mtio.h>
#include <scsi/scsi.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/sg.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>

/**
 * 转换指针类型。
 * 
 * @note 支持针地址偏移(Bytes)。
 * 
*/
#define GOOD_PTR2PTR(T,PTR,OF)   ((T*)(((char*)(PTR))+(OF)))

/**
 * 数值比较，返回最大值。
 * 
*/
#define GOOD_MAX(A,B)   (((A)>(B))?(A):(B))

/**
 * 数值比较，返回最小值。
 * 
*/
#define GOOD_MIN(A,B)   (((A)<(B))?(A):(B))

/**
 * 交换两个数值变量的值。
 * 
*/
#define GOOD_INTEGER_SWAP(A,B)  ({(A)^=(B);(B)^=(A);(A)^=(B);})

/**
 * 设置出错码，并返回。
*/
#define GOOD_ERRNO_AND_RETURN0(E) ({errno=(E);return;})

/**
 * 设置出错码，并返回值。
*/
#define GOOD_ERRNO_AND_RETURN1(E, V) ({errno=(E);return (V);})

/**
 * 计算数组大小。
*/
#define GOOD_ARRAY_SIZE(V)  (sizeof((V))/sizeof((V)[0]))


/*
 * 原子操作。
 *　
 * type __sync_fetch_and_add (type *ptr, type value);
 * type __sync_fetch_and_sub (type *ptr, type value);
 * type __sync_fetch_and_or (type *ptr, type value);
 * type __sync_fetch_and_and (type *ptr, type value);
 * type __sync_fetch_and_xor (type *ptr, type value);
 * type __sync_fetch_and_nand (type *ptr, type value);
 * type __sync_add_and_fetch (type *ptr, type value);
 * type __sync_sub_and_fetch (type *ptr, type value);
 * type __sync_or_and_fetch (type *ptr, type value);
 * type __sync_and_and_fetch (type *ptr, type value);
 * type __sync_xor_and_fetch (type *ptr, type value);
 * type __sync_nand_and_fetch (type *ptr, type value);
 * bool __sync_bool_compare_and_swap (type*ptr, type oldval, type newval, ...)
 * type __sync_val_compare_and_swap (type *ptr, type oldval,  type newval, ...)
*/ 

/**
 * 返回旧值。
*/
#define good_atomic_load(ptr)   __sync_and_and_fetch((ptr),*(ptr))

/**
 * 设置新值。
*/
#define good_atomic_store(ptr,newval)   __sync_val_compare_and_swap((ptr),*(ptr),(newval))

/**
 * 比较两个值。
*/
#define good_atomic_compare(ptr,oldval) __sync_bool_compare_and_swap((ptr),(oldval),*(ptr))

/**
 * 比较两个值，相同则用新值替换旧值。
*/
#define good_atomic_compare_and_swap(ptr,oldval,newval) __sync_bool_compare_and_swap

/**
 * 加法，返回旧值。
*/
#define good_atomic_fetch_and_add   __sync_fetch_and_add

#endif //GOOD_UTIL_DEFS_H