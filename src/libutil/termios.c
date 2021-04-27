/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "termios.h"

int good_tcattr_option(int fd, const struct termios *now, struct termios *old)
{
    assert(fd >=0 && (now != NULL || old != NULL));

    if (old)
    {
        if (tcgetattr(fd, old) != 0)
            goto final_error;
    }

    if (now)
    {
        if (tcsetattr(fd,TCSANOW,now)!=0)
            goto final_error;
    }

    return 0;

final_error:

    return -1;
}

int good_tcattr_cbreak(int fd, struct termios *old)
{
    struct termios now = {0};

    assert(fd >= 0);

    if (good_tcattr_option(fd, NULL, &now) != 0)
        return -1;

    now.c_lflag &= (~ICANON);
    now.c_lflag &= (~ECHO);
    now.c_cc[VTIME] = 0;
    now.c_cc[VMIN] = 1;

    return good_tcattr_option(fd,&now,old);
}