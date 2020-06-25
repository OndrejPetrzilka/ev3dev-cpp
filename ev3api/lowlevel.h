#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace ev3api
{
    namespace lowlevel
    {
        template <class T>
        class Attribute
        {
        private:
            FILE *m_handle;

        public:
            Attribute(const char *sysDev, const char *mode)
            {
                m_handle = fopen(sysDev, mode);
            }

            ~Attribute()
            {
                if (m_handle != nullptr)
                {
                    fclose(m_handle);
                    m_handle = nullptr;
                }
            }

            FILE *getHandle()
            {
                return m_handle;
            }

            bool isValid()
            {
                return m_handle != nullptr;
            }

            T getValue()
            {
                fseek(m_handle, 0);
                T result;
                fread(&result, sizeof(T), 1, m_handle);
            }

            void setValue(T value)
            {
                fseek(0);
                fwrite(&value, sizeof(T), 1, m_handle);
            }
        };
    } // namespace lowlevel

} // namespace ev3api