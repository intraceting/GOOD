/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_GENERAL_H
#define GOOD_UTIL_GENERAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include <pthread.h>
#include <string.h>
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

/*------------------------------------------------------------------------------------------------*/

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


/*------------------------------------------------------------------------------------------------*/

/**
 * 数值对齐。
 * 
 * @param align 对齐量。0,1是等价的。
*/
size_t good_align(size_t size,size_t align);

/**
 * 执行一次。
 * 
 * @param status 状态，一般是静态类型。必须初始化为0。
 * @param routine 执行函数。0 成功，!0 失败。
 * 
 * @return 0 第一次，1 第N次，-1 失败。
*/
int good_once(atomic_int* status,int (*routine)(void *opaque),void *opaque);

/*------------------------------------------------------------------------------------------------*/

/**
 * 内存申请。
 */
void* good_heap_alloc(size_t size);

/**
 * 内存重新申请。
 */
void* good_heap_realloc(void *buf,size_t size);

/**
 * 内存释放。
 * 
 * @param data 内存的指针。
 */
void good_heap_free(void *data);

/**
 * 内存释放。
 * 
 * @param data 指针的指针。返回时赋值NULL(0)。
 */
void good_heap_freep(void **data);

/**
 * 内存克隆。
*/
void* good_heap_clone(const void* data,size_t size);

/*------------------------------------------------------------------------------------------------*/

/**
 * 时间戳整形化。
 * 
 * 当精度为纳秒时，公元2444年之前或者时长544年之内有效。
 * 
 * @param precision 精度。0～9。
 * 
*/
uint64_t good_time_clock2kind(struct timespec* ts,uint8_t precision);

/**
 * 获取时间戳并整形化
 * 
 * @param id CLOCK_* in time.h
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

/*------------------------------------------------------------------------------------------------*/

/**
 * 字符串查找。
 * 
 * @param caseAb 0 不区分大小写，!0 区分大小写。
 * 
 * @return NULL(0) 未找到，!NULL(0) 匹配到的首地址。
*/
const char* good_strstr(const char *str, const char *sub,int caseAb);

/**
 * 字符串比较。
 * 
 * @param caseAb 0 不区分大小写，!0 区分大小写。
*/
int good_strcmp(const char *s1, const char *s2,int caseAb);

/**
 * 字符串比较。
 * 
 * @param caseAb 0 不区分大小写，!0 区分大小写。
*/
int good_strncmp(const char *s1, const char *s2,size_t len,int caseAb);

/**
 * 字符串修剪。
 * 
 * @param isctype_cb 字符比较函数。返回值：!0 是，0 否。
 * @param where 0 右端，1 左端，2 两端。
 * 
 * @see is*() functions. in ctype.h
*/
char* good_strtrim(char* str,int (*isctype_cb)(int c),int where);

/**
 * 字符串分割。
 * 
 * @param str 待分割的字符串。可能会被修改。
 * @param delim 分割字符串。全字匹配，并区分大小写。
 * @param saveptr 临时指针。不可支持访问。
 * 
 * @return NULL(0) 结束，!NULL(0) 分割后的字符串首地址。
*/
char *good_strtok(char *str, const char *delim, char **saveptr);

/**
 * 字符串匹配。
 * 
 * @return 0 成功，!0 失败。
*/
int good_fnmatch(const char *str,const char *wildcard,int caseAb,int ispath);

/**
 * 检测字符串中的字符类型。
 * 
 * @param isctype_cb 字符比较函数。返回值：!0 是，0 否。
 * 
 * @return !0 通过，0 未通过。
 * 
 * @see is*() functions. in ctype.h
*/
int good_strtype(const char* str,int (*isctype_cb)(int c));

/*------------------------------------------------------------------------------------------------*/

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

/*------------------------------------------------------------------------------------------------*/

/**
 * 字节序检测
 * 
 * @param big 0 检测是否为小端字节序，!0 检测是否为大端字节序。
 * 
 * @return 0 否，!0 是。
 */
int good_endian_check(int big);

/**
 * 字节序交换。
 * 
 * @return dst
*/
uint8_t* good_endian_swap(uint8_t* dst,int len);

/**
 * 网络字节序转本地字节序。
 * 
 * 如果本地是大端字节序，会忽略。
*/
uint8_t* good_endian_ntoh(uint8_t* dst,int len);

/**
 * 16位整型数值，网络字节序转本地字节序。
*/
uint16_t good_endian_ntoh16(uint16_t num);

/**
 * 32位整型数值，网络字节序转本地字节序。
*/
uint32_t good_endian_ntoh32(uint32_t num);

/**
 * 64位整型数值，网络字节序转本地字节序。
*/
uint64_t good_endian_ntoh64(uint64_t num);

/**
 * 本地字节序转网络字节序。
 * 
 * 如果本地是大端字节序，会忽略。
*/
uint8_t* good_endian_hton(uint8_t* dst,int len);

/**
 * 16位整型数值，本地字节序转网络字节序。
*/
uint16_t good_endian_hton16(uint16_t num);

/**
 * 32位整型数值，本地字节序转网络字节序。
*/
uint32_t good_endian_hton32(uint32_t num);

/**
 * 64位整型数值，本地字节序转网络字节序。
*/
uint64_t good_endian_hton64(uint64_t num);

/*------------------------------------------------------------------------------------------------*/

/** 
 * 布隆-插旗
 * 
 * @param size 池大小(Bytes)
 * @param number 编号。有效范围：0 ～ size*8-1。
 * 
 * @return 0 成功，1 成功（或重复操作）。
*/
int good_bloom_mark(uint8_t* pool,size_t size,size_t number);

/** 
 * 布隆-拔旗
 * 
 * @param size 池大小(Bytes)
 * @param number 编号。有效范围：0 ～ size*8-1。
 * 
 * @return 0 成功，1 成功（或重复操作）。
 * 
*/
int good_bloom_unset(uint8_t* pool,size_t size,size_t number);

/**
 * 布隆-过滤
 * 
 * @param size 池大小(Bytes)
 * @param number 编号。有效范围：0 ～ size*8-1。
 * 
 * @return 0 不存在，1 已存在。
*/
int good_bloom_filter(uint8_t* pool,size_t size,size_t number);

/*------------------------------------------------------------------------------------------------*/

/**
 * 拼接目录。
 * 
 * 自动检查前后的'/'字符，接拼位置只保留一个'/'字符，或自动添加一个'/'字符。
 * 
 * @warning 要有足够的可用空间，不然会溢出。
*/
char *good_dirdir(char *path,const char *suffix);

/**
 * 创建目录。
 * 
 * 支持创建多级目录。如果末尾不是'/'，则最后一级的名称会被当做文件名而忽略。
*/
void good_mkdir(const char *path,mode_t mode);

/**
 * 截取目录。
 * 
 * 最后一级的名称会被裁剪，并且无论目录结构是否真存在都会截取。 
*/
char *good_dirname(char *dst, const char *src);

/**
 * 截取目录或文件名称。
 * 
 * 最后一级的名称'/'(包括)之前的会被裁剪，并且无论目录结构是否真存在都会截取。 
*/
char *good_basename(char *dst, const char *src);

/**
 * 美化目录。
 * 
 * 不会检测目录结构是否存在。
 * 
 * 例：/aaaa/bbbb/../ccc -> /aaaa/ccc
 * 例：/aaaa/bbbb/./ccc -> /aaaa/bbbb/ccc
*/
char *good_dirnice(char *dst, const char *src);

/*------------------------------------------------------------------------------------------------*/

/**
 * 获取当前程序的完整路径和文件名。
*/
char* good_proc_pathfile(char* buf);

/**
 * 获取当前程序的完整路径。
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
 * 
*/
char* good_proc_dirname(char* buf,const char* append);

/**
 * 获取当前程序的文件名。
*/
char* good_proc_basename(char* buf);

/**
 * 单实例模式运行。
 * 
 * 文件句柄在退出前不要关闭，否则会使文件解除锁定状态。
 * 
 * 进程ID以十进制文本格式写入文件，例：2021 。
 * 
 * @param pid 当接口返回时，被赋值正在运行的进程ID。NULL(0) 忽略。
 * 
 * @return >= 0 文件句柄(当前进程是唯一进程)，-1 已有实例正在运行。
*/
int good_proc_singleton(const char* lockfile,int* pid);

/*------------------------------------------------------------------------------------------------*/

/**
 * 获取当前用户的运行路径。
 * 
 * 可能不存在，使用前最好检查一下。
 *
 * /var/run/user/$UID/
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
*/
char* good_user_dirname(char* buf,const char* append);

/*------------------------------------------------------------------------------------------------*/

/**
 * 在描述符上等待事件。
 * 
 * @param event 事件。0x01 读，0x02 写，0x03读写。
 * @param timeout 超时(毫秒)。>= 0 有事件或时间过期，< 0 直到有事件或出错。
 * 
 * @return > 0 有事件，0 超时，< 0 出错。
*/
int good_poll(int fd, int event,time_t timeout);

/**
 * 写数据。
 * 
 * @return > 0 写入的长度，<= 0 写入失败或空间不足。
*/
ssize_t good_write(int fd, const void *data, size_t size);

/**
 * 读数据。
 * 
 * @return > 0 读取的长度，<= 0 读取失败或已到末尾。
*/
ssize_t good_read(int fd, void *data, size_t size);

/**
 * 关闭文件句柄。
*/
void good_closep(int *fd);

/**
 * 打开文件。
 * 
 * @return >= 0 句柄，-1 失败。
 * 
*/
int good_open(const char *file, int rw, int nonblock, int create);

/**
 * 打开文件2。
 * 
 * 已打开的文件会被关闭，新打开的文件会绑定到fd2句柄。
 * 
 * @param fd2 已打开的句柄。
 * 
 * @return fd2 成功，-1 失败。
 * 
*/
int good_open2(int fd2,const char *file, int rw, int nonblock, int create);

/**
 * 获取标志。
 * 
 * @return !- 成功(标志)，-1 失败。
*/
int good_fflag_get(int fd);

/**
 * 添加标志。
 * 
 * @return 0 成功，-1 失败。
*/
int good_fflag_add(int fd,int flag);

/**
 * 删除标志。
 * 
 * @return 0 成功，-1 失败。
*/
int good_fflag_del(int fd,int flag);

/*------------------------------------------------------------------------------------------------*/

/**
 * 创建子进程，用于执行shell。
 *
 * @param cmd 命令行字符串指针。
 * @param envp 环境变量的组数指针，不影响父进程。{"KEY=VALUE","...",NULL}。
 * @param stdin_fd 输入句柄指针，NULL(0) 忽略。
 * @param stdout_fd 输出句柄指针，NULL(0) 忽略。
 * @param stderr_fd 出错句柄指针，NULL(0) 忽略。
 * 
 * @return 子进程ID 成功，-1 失败。
*/
pid_t good_popen(const char* cmd,char * const envp[],int* stdin_fd, int* stdout_fd, int* stderr_fd);


/*------------------------------------------------------------------------------------------------*/

/**
 * 打开共享内存文件。
 *
 * 通常是在'/dev/shm/'目录内创建。
 * 
 * @return >= 0 句柄，-1 失败。
*/
int good_shm_open(const char* name,int rw, int create);

/**
 * 删除共享内存文件。
 * 
 * @return 0 成功，-1 失败。
*/
int good_shm_unlink(const char* name);

/*------------------------------------------------------------------------------------------------*/

#endif //GOOD_UTIL_GENERAL_H