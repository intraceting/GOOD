/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "mman.h"

void _good_munmap_cb(size_t number,uint8_t **data,size_t* size,void *opaque)
{
    assert(data[0] != MAP_FAILED);
    assert(size[0] > 0);

    assert(munmap(data[0],size[0])==0);
}

good_buffer_t* good_mmap(int fd,int rw,int share)
{
    void* mmptr = MAP_FAILED;
    int prot = PROT_READ;
    int flags = MAP_PRIVATE;
    struct stat attr;

    good_buffer_t *buf = NULL;

    assert(fd>=0);

    if (fstat(fd, &attr) == -1)
        return NULL;

    if (attr.st_size <= 0)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, NULL);

    if(rw)
        prot = PROT_READ | PROT_WRITE;
    if(share)
        flags = MAP_SHARED;

    mmptr = mmap(0,attr.st_size,prot,flags,fd,0);
    if(mmptr == MAP_FAILED)
        return NULL;

    buf = good_buffer_alloc(NULL,1,NULL,NULL);
    if (buf)
    {
        /*
         * 绑定内存和特定的释放函数，用于支持引用计数器。
        */
        buf->data[0] = mmptr;
        buf->size[0] = attr.st_size;
        buf->free_cb = _good_munmap_cb;
    }

final:

    if(!buf && mmptr != MAP_FAILED)
        munmap(mmptr,attr.st_size);

    return buf;
}

good_buffer_t *good_mmap2(const char *name, int rw, int share)
{
    int fd = -1;

    good_buffer_t *buf = NULL;

    assert(name);

    fd = good_open(name, rw, 0, 0);
    if (fd < 0)
        return NULL;

    buf = good_mmap(fd,rw,share);

    good_closep(&fd);
    
    return buf;
}

int good_msync(good_buffer_t *buf, int async)
{
    int flags = MS_SYNC;

    assert(buf);
    assert(buf->data[0] != MAP_FAILED && buf->size[0] > 0);

    if (async)
        flags = MS_ASYNC;

    return msync(buf->data[0], buf->size[0], flags);
}

void good_munmap(good_buffer_t** buf)
{
    if(!buf || !*buf)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    good_buffer_unref(buf);
}

int good_shm_open(const char* name,int rw, int create)
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

int good_shm_unlink(const char* name)
{
    assert(name);

    return shm_unlink(name);
}