#pragma once
#include <string>
#include "serializer.h"
#include "textUtils.h"

namespace ev3api
{
    class SerializerTextInt
    {
    public:
        static int read(FILE *handle, int *buffer, int count = 1)
        {
            // Only one string is supported
            if (count > 0)
            {
                // Longest possible value, 11 chars: -2147483648
                char buf[12];
                int read = fread(buf, sizeof(char), 11, handle);
                *buffer = atoi(buf);
                return read > 0 ? 1 : 0;
            }
            return 0;
        }

        static int write(FILE *handle, const int *buffer, int count = 1)
        {
            // Only one string is supported
            if (count > 0)
            {
                char buf[12] = {};
                modp_itoa10(*buffer, buf);
                return fwrite(buf, sizeof(char) * 12, 1, handle);
            }
            return 0;
        }
    };
} // namespace ev3api