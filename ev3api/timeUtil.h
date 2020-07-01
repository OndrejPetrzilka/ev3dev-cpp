#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>  //I/O system handling
#include <time.h>     //system time
#include <fcntl.h>    //open function
#include <unistd.h>   //close function

#define BILLION 1000000000

class Time
{
public:
    struct timespec ts;

    static Time now()
    {
        Time time;
        timespec_get(&time.ts, TIME_UTC);
        return time;
    }

    double seconds()
    {
        return ts.tv_sec + ts.tv_nsec / (double)BILLION;
    }

    double elapsedSeconds()
    {
        return elapsed().seconds();
    }

    double elapsedMiliseconds()
    {
        return elapsedSeconds() * 1000.0;
    }

    Time elapsed()
    {
        return diff(*this, now());
    }

    static Time add(Time t1, Time t2)
    {
        Time result;
        result.ts.tv_sec = t2.ts.tv_sec + t1.ts.tv_sec;
        result.ts.tv_nsec = t2.ts.tv_nsec + t1.ts.tv_nsec;
        if (result.ts.tv_nsec >= BILLION)
        {
            result.ts.tv_nsec -= BILLION;
            result.ts.tv_sec++;
        }
        return result;
    }

    static Time diff(Time oldTime, Time time)
    {
        Time result;
        if (time.ts.tv_nsec < oldTime.ts.tv_nsec)
        {
            result.ts.tv_sec = time.ts.tv_sec - 1 - oldTime.ts.tv_sec;
            result.ts.tv_nsec = BILLION + time.ts.tv_nsec - oldTime.ts.tv_nsec;
        }
        else
        {
            result.ts.tv_sec = time.ts.tv_sec - oldTime.ts.tv_sec;
            result.ts.tv_nsec = time.ts.tv_nsec - oldTime.ts.tv_nsec;
        }
        return result;
    }
};