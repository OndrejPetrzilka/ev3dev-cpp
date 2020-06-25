#pragma once
#include "serializer.h"
#include <string>

namespace ev3api
{
    template <>
    class Serializer<std::string>
    {
    public:
        static int read(FILE *handle, std::string *buffer, int count = 1)
        {
            // Only one string is supported
            if (count > 0)
            {
                // Attributes are typically 4KB file, ends by newline, padded by zeros
                const int readSize = 64;
                char buf[readSize + 1];
                buf[readSize] = '\0';

                int readLen = 0;
                buffer[0] = std::string();
                while (buffer->length() == readLen)
                {
                    buf[0] = '\0';
                    int read = fread(buf, sizeof(char), readSize, handle);
                    read = findBreak(buf, read);
                    readLen += readSize;
                    buffer->append(buf, read);
                }
                return 1;
            }
            return 0;
        }

        static int write(FILE *handle, const std::string *buffer, int count = 1)
        {
            // Only one string is supported
            if (count > 0)
            {
                return fwrite(buffer->c_str(), sizeof(char) * buffer->length(), 1, handle);
            }
            return 0;
        }
    };
} // namespace ev3api