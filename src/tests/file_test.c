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
#include "libutil/file.h"
#include "libutil/string.h"

void test_dir()
{
    char* path = good_heap_alloc(PATH_MAX);
    char* path2 = good_heap_alloc(PATH_MAX);
    char* path3 = good_heap_alloc(PATH_MAX);
    char* path4 = good_heap_alloc(PATH_MAX);
    char* path5 = good_heap_alloc(PATH_MAX);

    good_dirdir(path,"/tmp/abcd/efg/heigh//dfgsdfg///../..////////aaa.txt");

    realpath(path,path5);

    good_mkdir(path,0);

    good_dirname(path2,path);

 //   good_dirname(path5,path2);

    good_basename(path3,path);

    printf("%s : %s\n",path2,path3);
    printf("%s\n",path5);

    good_heap_free(path);
    good_heap_free(path2);
    good_heap_free(path3);
    good_heap_free(path5);

    strcpy(path4,"              asdfasdfsad     \r\n\t");

    printf("[%s]\n",path4);

    good_strtrim(path4,iscntrl,2);
    good_strtrim(path4,isblank,2);

    printf("[%s]\n",path4);



    good_heap_free(path4);
}

void test_file(const char* f1,const char* f2)
{
    good_mkdir(f2,0);

    int fd1 = good_open(f1,0,1,0);
    int fd2 = good_open(f2,1,1,1);

    assert(fd1 >= 0 && fd2 >= 0);

    good_buffer_t *rbuf = good_buffer_alloc3(512*1024);
    good_buffer_t *wbuf = good_buffer_alloc3(256);

    while (1)
    {
        char buf[1023];
        ssize_t rsize = good_read(fd1,buf,1023,rbuf);
        if(rsize<=0)
            break;
        
        ssize_t wsize = good_write(fd2,buf,rsize,wbuf);

        assert(wsize == rsize);
    }
    
    assert(good_write_trailer(fd2,1,0,wbuf)==0);

  //  assert(good_write_trailer(fd2,0,0,wbuf)==0);

    good_buffer_unref(&rbuf);
    good_buffer_unref(&wbuf);

    good_closep(&fd1);
    good_closep(&fd2);
}


int main(int argc, char **argv)
{
    
    test_dir();

    if(argc>=3)
        test_file(argv[1],argv[2]);

    return 0;
}