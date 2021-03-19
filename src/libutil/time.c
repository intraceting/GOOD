/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "time.h"

uint64_t good_time_clock2kind(struct timespec *ts, uint8_t precision)
{
    uint64_t kind = 0;
    uint64_t p = 0;

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

uint64_t good_time_clock2kind_with(clockid_t id,uint8_t precision)
{
    struct timespec ts = {0};

    assert(clock_gettime(id, &ts)==0);

    return good_time_clock2kind(&ts,precision);
}

struct tm *good_time_local2utc(struct tm *dst, const struct tm *src, int reverse)
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

struct tm* good_time_get(struct tm* tm,int utc)
{
    struct timespec ts = {0};

    assert(tm);

    clock_gettime(CLOCK_REALTIME,&ts);

    return (utc?gmtime_r(&ts.tv_sec,tm):localtime_r(&ts.tv_sec,tm));
}