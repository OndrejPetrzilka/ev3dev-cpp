#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

namespace ev3api
{
    class EV3Display
    {
    public:
        void clear()
        {
            // Clear screen, return cursor to top-left
            printf("\033[H\033[J");
        }

        // Sets display font, eg.: Uni2-VGA16, Lat2-Terminus16, Lat15-Terminus24x12
        // A full list of fonts can be found with `ls /usr/share/consolefonts`
        // 16px font = 7 lines, 24px font = 4 lines
        void setFont(const char* fontName)
        {
            char buf[64];
            snprintf(buf, sizeof buf, "setfont %s", fontName);
	        system(buf);
        }

        int print(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            int r = vprintf(format, args);
            va_end(args);
            return r;
        }

        int printClear(const char* format, ...)
        {
            clear();
            va_list args;
            va_start(args, format);
            int r = vprintf(format, args);
            va_end(args);
            return r;
        }
    };

    EV3Display display;
}