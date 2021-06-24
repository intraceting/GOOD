/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "mman.h"

static void _abtk_munmap_cb(abtk_allocator_t *alloc, void *opaque)
{
    int chk;

    assert(alloc);
    assert(alloc->pptrs[0] != MAP_FAILED);
    assert(alloc->sizes[0] > 0);

    chk = munmap(alloc->pptrs[0], alloc->sizes[0]);
    assert(chk == 0);
}

abtk_allocator_t* abtk_mmap(int fd,int rw,int shared)
{
    void* mmptr = MAP_FAILED;
    int prot = PROT_READ;
    int flags = MAP_PRIVATE;
    struct stat attr;

    abtk_allocator_t *alloc = NULL;

    assert(fd>=0);

    if (fstat(fd, &attr) == -1)
        return NULL;

    if (attr.st_size <= 0)
        ABTK_ERRNO_AND_RETURN1(ENOSPC, NULL);

    if(rw)
        prot = PROT_READ | PROT_WRITE;
    if(shared)
        flags = MAP_SHARED;

    mmptr = mmap(0,attr.st_size,prot,flags,fd,0);
    if(mmptr == MAP_FAILED)
        return NULL;

    alloc = abtk_allocator_alloc(NULL,1,0);
    if (alloc)
    {
        /*绑定内存和特定的释放函数，用于支持引用计数器。*/
        alloc->pptrs[0] = mmptr;
        alloc->sizes[0] = attr.st_size;

        /* 注册特定的析构函数。 */
        abtk_allocator_atfree(alloc,_abtk_munmap_cb,NULL);
    }

final:

    if(!alloc && mmptr != MAP_FAILED)
        munmap(mmptr,attr.st_size);

    return alloc;
}

abtk_allocator_t *abtk_mmap2(const char *name, int rw, int shared)
{
    int fd = -1;

    abtk_allocator_t *alloc = NULL;

    assert(name);

    fd = abtk_open(name, rw, 0, 0);
    if (fd < 0)
        return NULL;

    alloc = abtk_mmap(fd,rw,shared);

    abtk_closep(&fd);
    
    return alloc;
}

int abtk_msync(abtk_allocator_t *alloc, int async)
{
    int flags;

    assert(alloc);
    assert(alloc->pptrs[0] != MAP_FAILED && alloc->sizes[0] > 0);

    flags = (async?MS_ASYNC:MS_SYNC);

    return msync(alloc->pptrs[0], alloc->sizes[0], flags);
}

void abtk_munmap(abtk_allocator_t** alloc)
{
    if(!alloc || !*alloc)
        ABTK_ERRNO_AND_RETURN0(EINVAL);

    abtk_allocator_unref(alloc);
}
