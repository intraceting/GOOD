/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_NOTIFY_H
#define GOOD_UTIL_NOTIFY_H

#include "general.h"
#include "buffer.h"

/**
 * 监视器事件
 * 
*/
typedef struct _good_notify_event
{
    /**
     * 缓存。
     * 
     * 由调用者申请和释放。
    */
    good_buffer_t *buf;

    /**
     * 事件。
    */
    struct inotify_event event;

    /** 
     * 名字。
    */
    char name[PATH_MAX];

} good_notify_event_t;

/**
 * 初始化监视器。
 * 
 * @param nonblock 0 阻塞模式，!0 非阻塞模式。
 * 
 * @return >= 0 句柄，< 0 错误。
 * 
 * @see inotify_init1()
*/
int good_notify_init(int nonblock);

/**
 * 添加一个监视对象(文件或目录)。
 * 
 * @return >= 0 成功(WD)，< 0 错误。
 * 
 * @see inotify_add_watch()
*/
int good_notify_add(int fd,const char* name,uint32_t masks);

/**
 * 删除一个监视对象。
 * 
 * @return 0 成功，!0 失败。
 * 
 * @see inotify_rm_watch()
*/
int good_notify_remove(int fd,int wd);

/**
 * 监视。
 * 
 * 阻塞模式的句柄，响应可能会有延迟。
 * 
 * @param timeout 超时。>= 0 事件到来或时间过期返回，< 0 直到事件到来返回。
 * 
 * @return 0 成功，!0 超时或失败。
 * 
 * @see good_poll()
 * @see good_read()
*/
int good_notify_watch(int fd,good_notify_event_t *event,time_t timeout);


#endif //GOOD_UTIL_NOTIFY_H