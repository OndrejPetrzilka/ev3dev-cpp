#pragma once

#include <time.h>
#include <errno.h>

namespace ev3api
{
    // Sleep for the requested number of milliseconds.
    class Utils
    {
    public:
        void msleep(long msec)
        {
            if (msec < 0)
            {
                return;
            }

            struct timespec ts;
            ts.tv_sec = msec / 1000;
            ts.tv_nsec = (msec % 1000) * 1000000;

            while (nanosleep(&ts, &ts) && errno == EINTR)
            {
            }
        }
    };

    Utils utils;
} // namespace ev3api