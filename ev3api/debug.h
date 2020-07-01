#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

namespace ev3api
{
    class EV3Debug
    {
    public:
        int printLine(const char *format, ...)
        {
            va_list args;
            va_start(args, format);
            int r = vfprintf(stderr, format, args);
            fprintf(stderr, "\n");
            va_end(args);
            return r;
        }

        int print(const char *format, ...)
        {
            va_list args;
            va_start(args, format);
            int r = vfprintf(stderr, format, args);
            va_end(args);
            return r;
        }
    };

    EV3Debug debug;

    // convert a string represenation of an IP address into its numeric equivalent
    uint32_t Inet_AtoN(const char *buf)
    {
        // net_server inexplicably doesn't have this function; so I'll just fake it
        uint32_t ret = 0;
        int shift = 24; // fill out the MSB first
        bool startQuad = true;
        while ((shift >= 0) && (*buf))
        {
            if (startQuad)
            {
                unsigned char quad = (unsigned char)atoi(buf);
                ret |= (((uint32_t)quad) << shift);
                shift -= 8;
            }
            startQuad = (*buf == '.');
            buf++;
        }
        return ret;
    }
} // namespace ev3api