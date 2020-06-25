#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace ev3api
{
    template <class T>
    class Serializer
    {
    public:
        static int read(FILE *handle, T *buffer, int count = 1)
        {
            return fread(buffer, sizeof(T), count, handle);
        }

        static int write(FILE *handle, const T *buffer, int count = 1)
        {
            return fwrite(buffer, sizeof(T), count, handle);
        }
    };
}