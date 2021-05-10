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
#define GOOD_PTR2PTR(T, PTR, OF) ((T *)(((char *)(PTR)) + (OF)))

/**
 * 转换指针对象。
 * 
 * @note 支持针地址偏移(Bytes)。
 * 
*/
#define GOOD_PTR2OBJ(T, PTR, OF) (*GOOD_PTR2PTR(T,PTR,OF))

/**
 * 数值比较，返回最大值。
 * 
*/
#define GOOD_MAX(A, B) (((A) > (B)) ? (A) : (B))

/**
 * 数值比较，返回最小值。
 * 
*/
#define GOOD_MIN(A, B) (((A) < (B)) ? (A) : (B))

/**
 * 交换两个数值变量的值。
 * 
*/
#define GOOD_INTEGER_SWAP(A, B) ({(A)^=(B);(B)^=(A);(A)^=(B); })

/**
 * 设置出错码，并返回。
*/
#define GOOD_ERRNO_AND_RETURN0(E) ({errno=(E);return; })

/**
 * 设置出错码，并返回值。
*/
#define GOOD_ERRNO_AND_RETURN1(E, V) ({errno=(E);return (V); })

/**
 * 设置出错码，并跳转。
*/
#define GOOD_ERRNO_AND_GOTO1(E, M) ({errno=(E);goto M; })

/**
 * 计算数组大小。
*/
#define GOOD_ARRAY_SIZE(V) (sizeof((V)) / sizeof((V)[0]))


#endif //GOOD_UTIL_DEFS_H