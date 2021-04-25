/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "libutil/dirent.h"
#include "libutil/mman.h"
#include "libutil/notify.h"
#include "libutil/iconv.h"
#include "libutil/tar.h"

void test_dir()
{
    char* path = good_heap_alloc(PATH_MAX);
    char* path2 = good_heap_alloc(PATH_MAX);
    char* path3 = good_heap_alloc(PATH_MAX);
    char* path4 = good_heap_alloc(PATH_MAX);
    char* path5 = good_heap_alloc(PATH_MAX);
    char* path6 = good_heap_alloc(PATH_MAX);

    good_dirdir(path,"/tmp/abcd/efg/heigh/././././///////sadfasdfasdfasfdsd/.a.a.sdf.../asdfasdfasfd/.../../../dfgsdfg///../..////////aaa.txt");

    good_dirnice(path5,path);

    good_mkdir(path5,0);

    good_dirname(path2,path);

    good_dirname(path6,path5);

    good_basename(path3,path);

    printf("%s : %s\n",path2,path3);
    printf("%s : %s\n",path5,path6);

    good_heap_free(path);
    good_heap_free(path2);
    good_heap_free(path3);
    good_heap_free(path5);
    good_heap_free(path6);

    strcpy(path4,"              asdfasdfsad     \r\n\t");

    printf("[%s]\n",path4);

    good_strtrim(path4,iscntrl,2);
    good_strtrim(path4,isblank,2);

    printf("[%s]\n",path4);



    good_heap_free(path4);
}

int compare_cb(const good_tree_t *node1, const good_tree_t *node2, void *opaque)
{
    char* src = GOOD_PTR2PTR(char, node1->alloc->pptrs[GOOD_DIRENT_NAME], 0);
    char* dst = GOOD_PTR2PTR(char, node2->alloc->pptrs[GOOD_DIRENT_NAME], 0);

    return good_strcmp(src,dst,1);
}

void test_sort(good_tree_t *t,int by)
{
    good_tree_order_t o = {by,compare_cb,NULL};

    good_tree_sort(t,&o);
}

int dump2(size_t deep, good_tree_t *node, void *opaque)
{

    test_sort(node,0);

    char *path = (char*)(node->alloc->pptrs[GOOD_DIRENT_NAME]);
    struct stat *stat = (struct stat *)(node->alloc->pptrs[GOOD_DIRENT_STAT]);


    char name[NAME_MAX] ={0};
    good_basename(name,path);

#if 1
    good_tree_fprintf(stderr,deep,node,"%s%s\n",name,(S_ISDIR(stat->st_mode)?"/":""));
#else 
    good_tree_fprintf(stderr,deep,node,"%s(%s)\n",name,path);
#endif
    return 1;
}

void traversal(good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_iterator_t it = {0,dump2,NULL};
    good_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}

void test_dirscan()
{
    size_t sizes[2] = {PATH_MAX,sizeof(struct stat)};
    good_tree_t * t = good_tree_alloc2(sizes,2);

   strcpy(t->alloc->pptrs[GOOD_DIRENT_NAME],"/tmp/");
   good_dirscan(t,100,0);

//    strcpy(t->alloc->pptrs[GOOD_DIRENT_NAME],"/proc/");
 //   good_dirscan(t,5,0);

 //   strcpy(t->alloc->pptrs[GOOD_DIRENT_NAME],"/usr");
 //   good_dirscan(t,100,0);

 //   strcpy(t->alloc->pptrs[GOOD_DIRENT_NAME],"/mnt");
 //   good_dirscan(t,100,1);

 //   strcpy(t->alloc->pptrs[GOOD_DIRENT_NAME],"/tmp");
  //  good_dirscan(t,1,0);

    traversal(t);

    good_tree_free(&t);
    
}

void test_file(const char *f1, const char *f2)
{
    good_mkdir(f2, 0);

    int fd1 = good_open(f1, 0, 1, 0);
    int fd2 = good_open(f2, 1, 1, 1);

    assert(fd1 >= 0 && fd2 >= 0);

    good_buffer_t *rbuf = good_buffer_alloc2(512);
    good_buffer_t *wbuf = good_buffer_alloc2(256);

    while (1)
    {
        char buf[1023];
        ssize_t rsize = good_block_read(fd1, buf, 1023, rbuf);
        if (rsize <= 0)
            break;

        ssize_t wsize = good_block_write(fd2, buf, rsize, wbuf);

        assert(wsize == rsize);
    }

    assert(good_block_write_trailer(fd2,0, wbuf) == 0);

    good_buffer_freep(&rbuf);
    good_buffer_freep(&wbuf);

    good_closep(&fd1);
    good_closep(&fd2);
}

void test_mman()
{
    good_allocator_t * buf = good_mmap2("/tmp/test_mman.txt",1,1);

    /*支持引用访问。*/
    good_allocator_t * buf2 = good_allocator_refer(buf);

    memset(buf->pptrs[0],'A',buf->sizes[0]);

    good_munmap(&buf);

    assert(good_msync(buf2,0)==0);

    good_munmap(&buf2);


    /*如果映射的内存页面是私有模式，则对内存数据修改不会影响原文件。*/
    good_allocator_t * buf3 = good_mmap2("/tmp/test_mman.txt",1,0);

    memset(buf3->pptrs[0],'B',buf3->sizes[0]);

    assert(good_msync(buf3,0)==0);

    good_munmap(&buf3);

    /*创建共享内存文件。*/
    int fd = good_shm_open("test_mman",1,1);

    /*划拔点内存空间。*/
    ftruncate(fd,100);

    good_allocator_t * buf4 = good_mmap(fd,1,1);

    memset(buf4->pptrs[0],'C',buf4->sizes[0]);

    good_allocator_t * buf5  = good_allocator_refer(buf4);

    good_munmap(&buf4);

    assert(good_msync(buf5,0)==0);

    good_munmap(&buf5);

    good_allocator_t * buf6 = good_mmap(fd,1,0);

    printf("%s\n",buf6->pptrs[0]);

    good_munmap(&buf6);

    good_closep(&fd);

    /*删除*/
    good_shm_unlink("test_mman");
}

void test_notify()
{
    int fd = good_notify_init(1);

    good_notify_event_t t = {0};

    t.buf = good_buffer_alloc2(4096);

    //int wd = good_notify_add(fd,"/tmp/",IN_ALL_EVENTS);
    int wd = good_notify_add(fd,"/tmp/",IN_CREATE|IN_DELETE|IN_MOVE_SELF|IN_MOVE);

    for(;;)
    {
        

        if(good_notify_watch(fd,&t,-1)<0)
            break;

        if(t.event.mask & IN_ACCESS )
            printf("Access:");
        if(t.event.mask & IN_MODIFY )
            printf("Modify:");
        if(t.event.mask & IN_ATTRIB )
            printf("Metadata changed:");
        if(t.event.mask & IN_CLOSE )
            printf("Close:");
        if(t.event.mask & IN_OPEN )
            printf("Open:");
        if(t.event.mask & IN_MOVED_FROM )
            printf("Moved from(%u):",t.event.cookie);
        if(t.event.mask & IN_MOVED_TO )
            printf("Moved to(%u):",t.event.cookie);
        if(t.event.mask & IN_CREATE )
            printf("Created:");
        if(t.event.mask & IN_DELETE )
            printf("Deleted:");
        if(t.event.mask & IN_MOVE_SELF )
            printf("Deleted self:");
        if(t.event.mask & IN_UNMOUNT )
            printf("Umount:");
        if(t.event.mask & IN_IGNORED )
            printf("Ignored:");

        printf("%s\n",t.name);
    }

    good_buffer_freep(&t.buf);

    good_closep(&fd);
}

void test_iconv()
{
    char src[100] = {"我爱我家"};
    char dst[100] = {0};

    iconv_t cd = iconv_open("GBK","UTF8");

    size_t remain = 0;
    size_t n = good_iconv(cd,src,strlen(src),dst,4,&remain);

    iconv_close(cd);
}

void test_tar(const char* name)
{
    // int fd =good_open(name,0,0,0);

    // good_buffer_t *buf = good_buffer_alloc2(512 * 1024);


    // good_tar_hdr h;

    // good_tar_read(fd,&h,512,buf);

    // assert(good_tar_hdr_verify(&h));


    // good_buffer_freep(&buf);

    // good_closep(&fd);
}

int main(int argc, char **argv)
{
    
    // test_dir();

   //  test_dirscan();

     if(argc>=3)
         test_file(argv[1],argv[2]);

    // test_mman();

    //test_notify();

   // test_iconv();

 //  if(argc>=2)
 //       test_tar(argv[1]);

    return 0;
}