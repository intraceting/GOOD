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
#include "goodutil/log.h"

void agent(void *opaque,int level,const char* fmt,va_list args)
{
    fprintf(stderr,"LV[%d]: ",level);
    vfprintf(stderr,fmt,args);
}

int main(int argc, char **argv)
{

    good_log_init(NULL,1);

  //  good_log_redirect(agent,NULL);

    good_log_mask(LOG_UPTO(LOG_DEBUG));

    for(int i = LOG_EMERG ;i<= LOG_DEBUG;i++)
        good_log_printf(i,"haha-%d",i);

    return 0;
}