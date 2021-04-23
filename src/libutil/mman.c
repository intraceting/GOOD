/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "mman.h"

static void _good_munmap_cb(good_allocator_t *alloc, void *opaque)
{
    assert(alloc);
    assert(alloc->pptrs[0] != MAP_FAILED);
    assert(alloc->sizes[0] > 0);

    assert(munmap(alloc->pptrs[0],alloc->sizes[0])==0);
}

good_allocator_t* good_mmap(int fd,int rw,int shared)
{
    void* mmptr = MAP_FAILED;
    int prot = PROT_READ;
    int flags = MAP_PRIVATE;
    struct stat attr;

    good_allocator_t *alloc = NULL;

    assert(fd>=0);

    if (fstat(fd, &attr) == -1)
        return NULL;

    if (attr.st_size <= 0)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, NULL);

    if(rw)
        prot = PROT_READ | PROT_WRITE;
    if(shared)
        flags = MAP_SHARED;

    mmptr = mmap(0,attr.st_size,prot,flags,fd,0);
    if(mmptr == MAP_FAILED)
        return NULL;

    alloc = good_allocator_alloc(NULL,1);
    if (alloc)
    {
        /*绑定内存和特定的释放函数，用于支持引用计数器。*/
        alloc->pptrs[0] = mmptr;
        alloc->sizes[0] = attr.st_size;

        /* 注册特定的析构函数。 */
        good_allocator_atfree(alloc,_good_munmap_cb,NULL);
    }

final:

    if(!alloc && mmptr != MAP_FAILED)
        munmap(mmptr,attr.st_size);

    return alloc;
}

good_allocator_t *good_mmap2(const char *name, int rw, int shared)
{
    int fd = -1;

    good_allocator_t *alloc = NULL;

    assert(name);

    fd = good_open(name, rw, 0, 0);
    if (fd < 0)
        return NULL;

    alloc = good_mmap(fd,rw,shared);

    good_closep(&fd);
    
    return alloc;
}

int good_msync(good_allocator_t *alloc, int async)
{
    int flags;

    assert(alloc);
    assert(alloc->pptrs[0] != MAP_FAILED && alloc->sizes[0] > 0);

    flags = (async?MS_ASYNC:MS_SYNC);

    return msync(alloc->pptrs[0], alloc->sizes[0], flags);
}

void good_munmap(good_allocator_t** alloc)
{
    if(!alloc || !*alloc)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    good_allocator_unref(alloc);
}
