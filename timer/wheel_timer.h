#ifndef TIME_WHEEL_TIMER_H
#define TIME_WHEEL_TIMER_H

#include <sys/socker.h>

class wheel_timer;

struct client_wheel_time
{
    sockaddr_in address;
    int sockfd;
    wheel_timer *timer;
};

class wheel_timer
{
public:
    wheel_timer(int rot, int st):rotation(
};

#endif
