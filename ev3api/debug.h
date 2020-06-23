#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

namespace ev3api
{
    class EV3Debug
    {
    public:
        int print(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            int r = vfprintf(stderr, format, args);
            va_end(args);
            return r;
        }
    };

    EV3Debug debug;
}