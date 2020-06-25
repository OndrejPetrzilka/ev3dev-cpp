#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

namespace ev3api
{
    class AttributeFile
    {
    private:
        FILE *m_handle;

    public:
        ~AttributeFile()
        {
            close();
        }

        // Open device attribute for reading/writing, returns true when it's opened
        bool open(const char *devicePath, const char *attributeName, const char *fileOpenMode = "r+")
        {
            if (m_handle == nullptr)
            {
                char buf[strlen(devicePath) + strlen(attributeName) + 1];
                strcpy(buf, devicePath);
                strcat(buf, attributeName);
                m_handle = fopen(buf, fileOpenMode);
                if (m_handle != nullptr)
                {
                    setbuf(m_handle, nullptr);
                    return true;
                }
                return false;
            }
            return true;
        }

        void close()
        {
            if (m_handle != nullptr)
            {
                fclose(m_handle);
                m_handle = nullptr;
            }
        }

        inline FILE *getHandle() const
        {
            return m_handle;
        }

        inline bool isOpen() const
        {
            return m_handle != nullptr;
        }
    };

} // namespace ev3api