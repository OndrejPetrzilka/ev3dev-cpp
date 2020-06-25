#pragma once
#include <stdio.h>
#include <string.h>
#include "serializer.h"

namespace ev3api
{
    template <>
    class Serializer<const char *>
    {
    public:
        static int write(FILE *handle, const char *const *value, int count = 1)
        {
            // Only one string is supported
            if (count > 0)
            {
                return fwrite(value[0], sizeof(char) * (strlen(value[0]) + 1), 1, handle);
            }
            return 0;
        }
    };
} // namespace ev3api