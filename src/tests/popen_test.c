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
#include "libutil/process.h"
#include "libutil/popen.h"

int main(int argc, char **argv)
{
    

    int i = -1;
    int o = -1;
    int p = good_popen("bash",&o,&i,NULL,NULL);

    char cmd[]={"ls /tmp/ -l"};
    good_write(o,cmd,strlen(cmd),NULL);
    good_write(o,"\n",1,NULL);

    while(1)
    {
        char buf[2]={0};
        ssize_t chk = good_read(i,buf,1,NULL);
        if(chk<=0)
            break;

        printf("%s",buf);

    }

    printf("aa\n");

    waitpid(p,NULL,0);

    return 0;
}