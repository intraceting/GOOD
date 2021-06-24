/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "general.h"

/*------------------------------------------------------------------------------------------------*/

/*
 * 检查内外版本号是否一致。
*/
#if ABTK_VERSION_MAJOR != BUILD_VERSION_MAJOR
#error "ABTK_VERSION_MAJOR must be equal to BUILD_VERSION_MAJOR"
#endif

/*
 * 检查内外版本号是否一致。
*/
#if ABTK_VERSION_MINOR != BUILD_VERSION_MINOR
#error "ABTK_VERSION_MINOR must be equal to BUILD_VERSION_MINOR"
#endif


/*------------------------------------------------------------------------------------------------*/

size_t abtk_align(size_t size, size_t align)
{
    size_t padding = 0;

    if (align > 1)
        size +=  align - (size % align);

    return size;
}

int abtk_once(int *status, int (*routine)(void *opaque), void *opaque)
{
    int chk, ret;

    assert(status != NULL && opaque != NULL);

    if (abtk_atomic_compare_and_swap(status,0, 1))
    {
        ret = 0;

        chk = routine(opaque);

        abtk_atomic_store(status, ((chk == 0) ? 2 : 0));
    }
    else
    {
        ret = 1;

        while (abtk_atomic_load(status) == 1)
            pthread_yield();
    }

    chk = ((abtk_atomic_load(status) == 2) ? 0 : -1);

    return (chk == 0 ? ret : -1);
}

/*------------------------------------------------------------------------------------------------*/

void *abtk_heap_alloc(size_t size)
{
    assert(size > 0);

    return calloc(1,size);
}

void* abtk_heap_realloc(void *buf,size_t size)
{
    assert(size > 0);

    return realloc(buf,size);
}

void abtk_heap_free(void *data)
{
    if (data)
        free(data);
}

void abtk_heap_freep(void **data)
{
    if (!data || !*data)
        ABTK_ERRNO_AND_RETURN0(EINVAL);

    abtk_heap_free(*data);
    *data = NULL;
}

void *abtk_heap_clone(const void *data, size_t size)
{
    void *buf = NULL;

    assert(data && size > 0);

    buf = abtk_heap_alloc(size);
    if (!buf)
        ABTK_ERRNO_AND_RETURN1(EINVAL, NULL);

    memcpy(buf, data, size);

    return buf;
}

/*------------------------------------------------------------------------------------------------*/


uint64_t abtk_time_clock2kind(struct timespec *ts, uint8_t precision)
{
    uint64_t kind = 0;
    uint64_t p = 0;

    assert(ts);

    if (precision <= 9 && precision >= 1)
    {
        p = powl(10, precision);

        kind = ts->tv_sec * p;
        kind += ts->tv_nsec / (1000000000 / p);
    }
    else
    {
        kind = ts->tv_sec;
    }

    return kind;
}

uint64_t abtk_time_clock2kind_with(clockid_t id,uint8_t precision)
{
    struct timespec ts = {0};

    assert(clock_gettime(id, &ts)==0);

    return abtk_time_clock2kind(&ts,precision);
}

struct tm *abtk_time_local2utc(struct tm *dst, const struct tm *src, int reverse)
{
    time_t sec = 0;

    assert(dst && src);

    if (reverse)
    {
        sec = timegm((struct tm*)src);
        localtime_r(&sec,dst);
    }
    else
    {
        sec = timelocal((struct tm *)src);
        gmtime_r(&sec, dst);
    }

    return dst;
}

struct tm* abtk_time_get(struct tm* tm,int utc)
{
    struct timespec ts = {0};

    assert(tm);

    clock_gettime(CLOCK_REALTIME,&ts);

    return (utc?gmtime_r(&ts.tv_sec,tm):localtime_r(&ts.tv_sec,tm));
}

/*------------------------------------------------------------------------------------------------*/

int isodigit(int c)
{
    return ((c >= '0' && c <= '7') ? 1 : 0);
}

/*------------------------------------------------------------------------------------------------*/

const char *abtk_strstr(const char *str, const char *sub, int caseAb)
{
    assert(str != NULL && sub != NULL);

    if (caseAb)
        return strstr(str, sub);

    return strcasestr(str, sub);
}

int abtk_strcmp(const char *s1, const char *s2, int caseAb)
{
    assert(s1 != NULL && s2 != NULL);

    if (caseAb)
        return strcmp(s1, s2);

    return strcasecmp(s1, s2);
}

int abtk_strncmp(const char *s1, const char *s2, size_t len, int caseAb)
{
    assert(s1 != NULL && s2 != NULL && len > 0);

    if (caseAb)
        return strncmp(s1, s2, len);

    return strncasecmp(s1, s2, len);
}

char *abtk_strtrim(char *str, int (*isctype_cb)(int c),int where)
{
    char *tmp = NULL;
    size_t len = 0;
    size_t blklen = 0;

    assert(str && isctype_cb);

    tmp = str;
    len = strlen(str);

    if (len <= 0)
        goto final;

    if (0 == where)
    {
        while (*tmp)
            tmp++;

        while (isctype_cb(*(--tmp)))
            *tmp = '\0';
    }
    else if (1 == where)
    {
        while (isctype_cb(*(tmp)))
        {
            tmp++;
            blklen++;
        }

        if (blklen <= 0)
            goto final;

        for (size_t i = 0; i < len - blklen; i++)
            str[i] = str[i + blklen];

        for (size_t j = len - blklen; j < len; j++)
            str[j] = '\0';
    }
    else if (2 == where)
    {
        abtk_strtrim(str,isctype_cb,0);
        abtk_strtrim(str,isctype_cb,1);
    }

final:

    return str;
}

char *abtk_strtok(char *str,const char *delim, char **saveptr)
{
    char* prev = NULL;
    char* find = NULL;

    assert(str && delim && saveptr);

    if(*saveptr)
        prev = *saveptr;
    else 
        prev = str;

    find = (char *)abtk_strstr(prev, delim, 1);
    if (find)
    {
        *find = '\0';
        *saveptr = find + strlen(delim);
    }
    else if (*prev != '\0')
    {
        *saveptr = prev + strlen(prev);
    }
    else
    {
        prev = NULL;
        *saveptr = NULL;
    }

    return prev;
}

int abtk_fnmatch(const char *str,const char *wildcard,int caseAb,int ispath)
{
    int flag = 0;
    int chk = FNM_NOMATCH;

    assert(str && wildcard);

    if (caseAb)
        flag |= FNM_CASEFOLD;
    if (ispath)
        FNM_PATHNAME | FNM_PERIOD;

    chk = fnmatch(wildcard, str, flag);

    return ((chk==FNM_NOMATCH)?-1:0);
}

int abtk_strtype(const char* str,int (*isctype_cb)(int c))
{
    const char* s = NULL;

    assert(str && isctype_cb);

    s = str;

    if(*s == '\0')
        return 0;

    while(*s)
    {
        if(!isctype_cb(*s++))
            return 0;
    }

    return 1;
}

/*------------------------------------------------------------------------------------------------*/

uint32_t abtk_hash_bkdr(const void* data,size_t size)
{
    uint32_t seed = 131; /* 31 131 1313 13131 131313 etc.. */
    uint32_t hash = 0;

    assert(data && size>0);

    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + ABTK_PTR2OBJ(uint8_t,data,i);
    }

    return hash;
}

uint64_t abtk_hash_bkdr64(const void* data,size_t size)
{
    uint64_t seed = 13113131; /* 31 131 1313 13131 131313 etc.. */
    uint64_t hash = 0;

    assert(data && size>0);

    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + ABTK_PTR2OBJ(uint8_t,data,i);
    }

    return hash; 
}

/*------------------------------------------------------------------------------------------------*/

int abtk_endian_check(int big)
{
    long test = 1;

    if (big)
        return (*((char *)&test) != 1);

    return (*((char *)&test) == 1);
}

uint8_t *abtk_endian_swap(uint8_t *dst, int len)
{
    assert(dst);

    if (len == 2 || len == 3)
    {
        ABTK_INTEGER_SWAP(dst[0], dst[len - 1]);
    }
    else if (len == 4)
    {
        ABTK_INTEGER_SWAP(dst[0], dst[3]);
        ABTK_INTEGER_SWAP(dst[1], dst[2]);
    }
    else if (len == 8)
    {
        ABTK_INTEGER_SWAP(dst[0], dst[7]);
        ABTK_INTEGER_SWAP(dst[1], dst[6]);
        ABTK_INTEGER_SWAP(dst[2], dst[5]);
        ABTK_INTEGER_SWAP(dst[3], dst[4]);
    }
    else if( len > 1 )
    {
        /* 5,6,7,other,... */
        for (int i = 0; i < len; i++)
            ABTK_INTEGER_SWAP(dst[len - i - 1], dst[i]);
    }

    return dst;
}

uint8_t* abtk_endian_ntoh(uint8_t* dst,int len)
{
    if(abtk_endian_check(0))
        return abtk_endian_swap(dst,len);
    
    return dst;
}

uint16_t abtk_endian_ntoh16(uint16_t src)
{
    return *((uint16_t*)abtk_endian_ntoh((uint8_t*)&src,sizeof(src)));
}

uint32_t abtk_endian_ntoh24(const uint8_t* src)
{
    uint32_t dst = 0;

    if (abtk_endian_check(0))
    {
        memcpy(&dst, src, 3);
        abtk_endian_swap((uint8_t*)&dst, 3);
    }
    else
    {
        memcpy(&dst, src, 3);
        dst >>= 8;
    }

    return dst;
}

uint32_t abtk_endian_ntoh32(uint32_t src)
{
    return *((uint32_t*)abtk_endian_ntoh((uint8_t*)&src,sizeof(src)));
}

uint64_t abtk_endian_ntoh64(uint64_t src)
{
    return *((uint64_t*)abtk_endian_ntoh((uint8_t*)&src,sizeof(src)));
}

uint8_t* abtk_endian_hton(uint8_t* dst,int len)
{
    if (abtk_endian_check(0))
        return abtk_endian_swap(dst,len);

    return dst;
}

uint16_t abtk_endian_hton16(uint16_t src)
{
    return *((uint16_t *)abtk_endian_hton((uint8_t *)&src, sizeof(src)));
}

uint8_t* abtk_endian_hton24(uint8_t* dst,uint32_t src)
{
    if (abtk_endian_check(0))
    {
        memcpy(dst, &src, 3);
        abtk_endian_swap(dst, 3);
    }
    else
    {
        src <<= 8;
        memcpy(dst, &src, 3);
    }

    return dst;
}

uint32_t abtk_endian_hton32(uint32_t src)
{
    return *((uint32_t *)abtk_endian_hton((uint8_t *)&src, sizeof(src)));
}

uint64_t abtk_endian_hton64(uint64_t src)
{
    return *((uint64_t *)abtk_endian_hton((uint8_t *)&src, sizeof(src)));
}

/*------------------------------------------------------------------------------------------------*/

int abtk_bloom_mark(uint8_t *pool, size_t size, size_t number)
{
    assert(pool && size > 0 && size * 8 > number);

    size_t bloom_pos = number & 7;
    size_t byte_pos = number >> 3;
    size_t value = 1 << bloom_pos;

    if((pool[byte_pos] & value) != 0)
        ABTK_ERRNO_AND_RETURN1(EBUSY,1);

    pool[byte_pos] |= value;

    return 0;
}

int abtk_bloom_unset(uint8_t* pool,size_t size,size_t number)
{
    assert(pool && size > 0 && size * 8 > number);

    size_t bloom_pos = number & 7;
    size_t byte_pos = number >> 3;
    size_t value = 1 << bloom_pos;

    if((pool[byte_pos] & value) == 0)
        ABTK_ERRNO_AND_RETURN1(EIDRM,1);

    pool[byte_pos] &= (~value);

    return 0;
}

int abtk_bloom_filter(uint8_t* pool,size_t size,size_t number)
{
    assert(pool && size > 0 && size * 8 > number);

    size_t bloom_pos = number & 7;
    size_t byte_pos = number >> 3;
    size_t value = 1 << bloom_pos;

    if((pool[byte_pos] & value) != 0)
        return 1;

    return 0;
}

/*------------------------------------------------------------------------------------------------*/

char *abtk_dirdir(char *path, const char *suffix)
{
    size_t len = 0;

    assert(path != NULL && suffix != NULL);

    len = strlen(path);
    if (len > 0)
    {
        if ((path[len - 1] == '/') && (suffix[0] == '/'))
        {
            path[len - 1] = '\0';
            len -= 1;
        }
        else if ((path[len - 1] != '/') && (suffix[0] != '/'))
        {
            path[len] = '/';
            len += 1;
        }
    }

    /* 要有足够的可用空间，不然会溢出。 */
    strcat(path + len, suffix);

    return path;
}

void abtk_mkdir(const char *path, mode_t mode)
{
    size_t len = 0;
    char *tmp = NULL;
    int chk = 0;

    assert(path != NULL);

    len = strlen(path);
    if (len <= 0)
        ABTK_ERRNO_AND_RETURN0(EINVAL);

    tmp = (char *)abtk_heap_clone(path, len + 1);
    if (!tmp)
        ABTK_ERRNO_AND_RETURN0(ENOMEM);

    /* 必须允许当前用户具有读、写、访问权限。 */
    mode |= S_IRWXU;

    for (size_t i = 1; i < len; i++)
    {
        if (tmp[i] != '/')
            continue;

        tmp[i] = '\0';

        if (access(tmp, F_OK) != 0)
            chk = mkdir(tmp, mode & (S_IRWXU | S_IRWXG | S_IRWXO));

        tmp[i] = '/';

        if (chk != 0)
            break;
    }

    if (tmp)
        abtk_heap_freep((void**)&tmp);
}

char *abtk_dirname(char *dst, const char *src)
{
    char *find = NULL;
    char *path = NULL;

    assert(dst != NULL && src != NULL);

    path = (char *)abtk_heap_clone(src, strlen(src) + 1);
    if (!path)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, NULL);

    find = dirname(path);
    if (find)
        memcpy(dst, find, strlen(find) + 1);

    abtk_heap_freep((void**)&path);

    return dst;
}

char *abtk_basename(char *dst, const char *src)
{
    char *find = NULL;
    char *path = NULL;

    assert(dst != NULL && src != NULL);

    path = (char *)abtk_heap_clone(src, strlen(src) + 1);
    if (!path)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, NULL);

    find = basename(path);
    if (find)
        memcpy(dst, find, strlen(find) + 1);

    abtk_heap_freep((void**)&path);

    return dst;
}

char *abtk_dirnice(char *dst, const char *src)
{
    char *s = NULL;
    char *t = NULL;
    char *d = NULL;
    size_t deep = 0;
    size_t stack_size = 2048;
    char **stack;
    char *saveptr = NULL;

    assert(dst != NULL && src != NULL);

    stack = abtk_heap_alloc(stack_size * sizeof(char *));
    if (!stack)
        goto final;

    d = dst;
    s = abtk_heap_clone(src, strlen(src) + 1);

    if (s == NULL || *s == '\0')
        goto final;

    /*拆分目录，根据目录层级关系压入堆栈。*/
    while (1)
    {
        t = abtk_strtok(s, "/", &saveptr);
        if (!t)
            break;

        if (*t == '\0')
            continue;

        if (abtk_strcmp(t, ".", 1) == 0)
            continue;

        if (abtk_strcmp(t, "..", 1) == 0)
        {
            if (deep > 0)
                stack[--deep] = NULL;
        }
        else
        {
            assert(deep < stack_size);

            stack[deep++] = t;
        }
    }

    /* 拼接目录 */
    if (*src == '/')
        abtk_dirdir(dst, "/");

    for (size_t i = 0; i < deep; i++)
    {
        if (i > 0)
            abtk_dirdir(dst, "/");

        abtk_dirdir(dst, stack[i]);
    }

final:

    abtk_heap_freep((void **)&stack);
    abtk_heap_freep((void **)&s);

    return dst;
}

/*------------------------------------------------------------------------------------------------*/

char *abtk_proc_pathfile(char *buf)
{
    assert(buf);

    if (readlink("/proc/self/exe", buf, PATH_MAX) == -1)
        return NULL;

    return buf;
}

char *abtk_proc_dirname(char *buf, const char *append)
{
    char *tmp = NULL;

    assert(buf);

    tmp = abtk_heap_alloc(PATH_MAX);
    if (!tmp)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, NULL);

    if (abtk_proc_pathfile(tmp))
    {
        abtk_dirname(buf, tmp);

        if (append)
            abtk_dirdir(buf, append);
    }
    else
    {
        /* 这里的覆盖不会影响调用者。*/
        buf = NULL;
    }

    abtk_heap_freep((void **)&tmp);

    return buf;
}

char *abtk_proc_basename(char *buf)
{
    char *tmp = NULL;

    assert(buf);

    tmp = abtk_heap_alloc(PATH_MAX);
    if (!tmp)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, NULL);

    if (abtk_proc_pathfile(tmp))
    {
        abtk_basename(buf, tmp);
    }
    else
    {
        /*这里的覆盖不会影响调用者。*/
        buf = NULL;
    }

    abtk_heap_freep((void **)&tmp);

    return buf;
}

int abtk_proc_singleton(const char *lockfile,int* pid)
{
    int fd = -1;
    char strpid[16] = {0};

    assert(lockfile);

    fd = abtk_open(lockfile, 1, 0, 1);
    if (fd < 0)
        return -1;

    /* 通过尝试加独占锁来确定是否程序已经运行。*/
    if (flock(fd, LOCK_EX | LOCK_NB) == 0)
    {
        /* PID可视化，便于阅读。*/
        snprintf(strpid,15,"%d",getpid());

        /* 清空。*/
        ftruncate(fd, 0);

        /*写入文件。*/
        abtk_write(fd,strpid,strlen(strpid));
        fsync(fd);

        /*进程ID就是自己。*/
        if(pid)
           *pid = getpid();

        /* 走到这里返回锁定文件的句柄。*/
        return fd;
    }

    /* 程序已经运行，进程ID需要从锁定文件中读取。 */
    if(pid)
    {
        abtk_read(fd,strpid,12);

        if(abtk_strtype(strpid,isdigit))
            *pid = atoi(strpid);
        else
            *pid = -1;
    }

    /* 独占失败，关闭句柄，返回-1。*/
    abtk_closep(&fd);
    ABTK_ERRNO_AND_RETURN1(EPERM,-1);
}

/*------------------------------------------------------------------------------------------------*/

char *abtk_user_dirname(char *buf, const char *append)
{
    assert(buf);

    snprintf(buf, PATH_MAX, "/var/run/user/%d/", getuid());

    if (access(buf, R_OK | W_OK | X_OK | F_OK) != 0)
        ABTK_ERRNO_AND_RETURN1(ENOENT, NULL);

    if (append)
        abtk_dirdir(buf, append);

    return buf;
}

/*------------------------------------------------------------------------------------------------*/

int abtk_poll(int fd, int event,time_t timeout)
{
    struct pollfd arr = {0};

    assert(fd >= 0 && (event & 0x03));

    arr.fd = fd;
    arr.events = 0;
    
    if((event & 0x01))
        arr.events |= POLLIN;
    if((event & 0x02))
        arr.events |= POLLOUT;

    return poll(&arr, 1, (timeout >= INT32_MAX ? -1 : timeout));
}

ssize_t abtk_write(int fd, const void *data, size_t size)
{
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fd >= 0 && data && size > 0);

    wsize = write(fd, data, size);
    if (wsize > 0)
    {
        if (wsize < size)
        {
            /*有的系统超过2GB，需要分段落盘。*/
            wsize2 = abtk_write(fd, ABTK_PTR2PTR(void, data, wsize), size - wsize);
            if (wsize2 > 0)
                wsize += wsize2;
        }
    }

    return wsize;
}

ssize_t abtk_read(int fd, void *data, size_t size)
{
    ssize_t rsize = 0;
    ssize_t rsize2 = 0;

    assert(fd >= 0 && data && size > 0);

    rsize = read(fd, data, size);
    if (rsize > 0)
    {
        if (rsize < size)
        {
            /*有的系统超过2GB，需要分段读取。*/
            rsize2 = abtk_read(fd, ABTK_PTR2PTR(char, data, rsize), size - rsize);
            if (rsize2 > 0)
                rsize += rsize2;
        }
    }

    return rsize;
}

void abtk_closep(int *fd)
{
    if (!fd || *fd < 0)
        ABTK_ERRNO_AND_RETURN0(EINVAL);

    close(*fd);
    *fd = -1;
}

int abtk_open(const char *file, int rw, int nonblock, int create)
{
    int flag = O_RDONLY;
    mode_t mode = S_IRUSR | S_IWUSR;

    assert(file);

    if (rw)
        flag = O_RDWR;

    if (nonblock)
        flag |= O_NONBLOCK;

    if (rw && create)
        flag |= O_CREAT;

    flag |= __O_LARGEFILE;
    flag |= __O_CLOEXEC;

    return open(file, flag, mode);
}

int abtk_open2(int fd2, const char *file, int rw, int nonblock, int create)
{
    int fd = -1;
    int fd3 = -1;

    assert(fd2 >= 0);

    fd = abtk_open(file, rw, nonblock, create);
    if (fd < 0)
        return -1;

    fd3 = dup2(fd, fd2);

    /*必须要关闭，不然句柄就会丢失，造成资源泄露。*/
    abtk_closep(&fd);

    return fd3;
}

int abtk_fflag_get(int fd)
{
    assert(fd >= 0);

    return fcntl(fd, F_GETFL, 0);
}

int abtk_fflag_add(int fd, int flag)
{
    int old;
    int opt;

    assert(fd >= 0 && flag != 0);

    old = fcntl(fd, F_GETFL, 0);
    if (old == -1)
        return -1;

    opt = old | flag;

    return fcntl(fd, F_SETFL, opt);
}

int abtk_fflag_del(int fd, int flag)
{
    int old;
    int opt;

    assert(fd >= 0 && flag != 0);

    old = fcntl(fd, F_GETFL, 0);
    if (old == -1)
        return -1;

    opt = old & ~flag;

    return fcntl(fd, F_SETFL, opt);
}

/*------------------------------------------------------------------------------------------------*/

pid_t abtk_popen(const char *cmd,char * const envp[], int *stdin_fd, int *stdout_fd, int *stderr_fd)
{
    pid_t child = -1;
    int out2in_fd[2] = {-1, -1};
    int in2out_fd[2] = {-1, -1};
    int in2err_fd[2] = {-1, -1};

    assert(cmd);

    if (pipe(out2in_fd) != 0)
        goto final;

    if (pipe(in2out_fd) != 0)
        goto final;

    if (pipe(in2err_fd) != 0)
        goto final;

    child = fork();
    if (child < 0)
        goto final;

    if (child == 0)
    {
        if (stdin_fd)
            dup2(out2in_fd[0], STDIN_FILENO);
        else
            abtk_open2(STDIN_FILENO, "/dev/null", 0, 0, 0);

        abtk_closep(&out2in_fd[1]);
        abtk_closep(&out2in_fd[0]);
        
        if (stdout_fd)
            dup2(in2out_fd[1], STDOUT_FILENO);
        else
            abtk_open2(STDOUT_FILENO, "/dev/null", 1, 0, 0);

        abtk_closep(&in2out_fd[0]);
        abtk_closep(&in2out_fd[1]);
        
        if (stderr_fd)
            dup2(in2err_fd[1], STDERR_FILENO);
        else
            abtk_open2(STDERR_FILENO, "/dev/null", 1, 0, 0);

        abtk_closep(&in2err_fd[0]);
        abtk_closep(&in2err_fd[1]);

        /* 这个基本都支持。*/
        execle("/bin/sh", "sh", "-c", cmd,NULL,envp);

        /*也许永远也不可能到这里.*/
        _exit(127);
    }
    else
    {
        /*
        * 关闭不需要的句柄。
        */
        abtk_closep(&out2in_fd[0]);
        abtk_closep(&in2out_fd[1]);
        abtk_closep(&in2err_fd[1]);

        if (stdin_fd)
            *stdin_fd = out2in_fd[1];
        else
            abtk_closep(&out2in_fd[1]);

        if (stdout_fd)
            *stdout_fd = in2out_fd[0];
        else
            abtk_closep(&in2out_fd[0]);

        if (stderr_fd)
            *stderr_fd = in2err_fd[0];
        else
            abtk_closep(&in2err_fd[0]);
            
    }

final:

    if (child < 0)
    {
        abtk_closep(&out2in_fd[0]);
        abtk_closep(&out2in_fd[1]);
        abtk_closep(&in2out_fd[0]);
        abtk_closep(&in2out_fd[1]);
        abtk_closep(&in2err_fd[0]);
        abtk_closep(&in2err_fd[1]);
    }

    return child;
}

/*------------------------------------------------------------------------------------------------*/

int abtk_shm_open(const char* name,int rw, int create)
{
    int flag = O_RDONLY;
    mode_t mode = S_IRUSR | S_IWUSR;

    assert(name);

    if (rw)
        flag = O_RDWR;

    if (rw && create)
        flag |= O_CREAT;

    return shm_open(name,flag,mode);
}

int abtk_shm_unlink(const char* name)
{
    assert(name);

    return shm_unlink(name);
}

/*------------------------------------------------------------------------------------------------*/

void abtk_openlog(const char *ident,int level,int copy2stderr)
{
    openlog(ident, LOG_CONS | LOG_PID | (copy2stderr ? LOG_PERROR : 0), LOG_USER);

    setlogmask(LOG_UPTO(level));
}

/*------------------------------------------------------------------------------------------------*/