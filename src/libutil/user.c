/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "user.h"

char *good_user_dirname(char *buf, const char *append)
{
    assert(buf);

    snprintf(buf, PATH_MAX, "/var/run/user/%d/", getuid());

    if (access(buf, R_OK | W_OK | X_OK | F_OK) != 0)
        GOOD_ERRNO_AND_RETURN1(ENOENT, NULL);

    if (append)
        good_dirdir(buf, append);

    return buf;
}
