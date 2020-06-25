#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "attribute.h"

namespace ev3api
{
    extern constexpr char ATTRIBUTE_MODE_RW[] = "r+";
    extern constexpr char ATTRIBUTE_MODE_RO[] = "r";
    extern constexpr char ATTRIBUTE_MODE_WO[] = "w";

    template <class T>
    class AttributeAccessor
    {
    public:
        static void read(FILE *handle, T *buffer, int count = 1)
        {
            fread(buffer, sizeof(T), count, handle);
        }

        static void write(FILE *handle, const T *buffer, int count = 1)
        {
            fwrite(buffer, sizeof(T), count, handle);
        }
    };

    template <class T, const char *openMode>
    class AttributeInstanceBase : Attribute
    {
    public:
        const char *const AttributeName;

        AttributeInstanceBase(const char *attributeName)
            : AttributeName(attributeName)
        {
        }

        const char *getAttributeMode()
        {
            return openMode;
        }

        bool open(const char *devicePath)
        {
            return Attribute::open(devicePath, AttributeName, openMode);
        }

        bool read(const char *devicePath, T *buffer, int count = 1)
        {
            if (open(devicePath))
            {
                fseek(getHandle(), 0, SEEK_SET);
                AttributeAccessor<T>::read(getHandle(), buffer, count);
                return true;
            }
            return false;
        }

        bool write(const char *devicePath, const T *buffer, int count = 1)
        {
            if (open(devicePath))
            {
                fseek(getHandle(), 0, SEEK_SET);
                AttributeAccessor<T>::write(getHandle(), buffer, count);
                return true;
            }
            return false;
        }

        T getValue(const char *devicePath)
        {
            T result;
            return read(devicePath, &result) ? result : T();
        }

        bool setValue(const char *devicePath, const T &value)
        {
            return write(devicePath, &value);
        }
    };

    template <class T>
    class AttributeReadWrite : AttributeInstanceBase<T, ATTRIBUTE_MODE_RW>
    {
    public:
        AttributeReadWrite(const char *attributeName)
            : AttributeInstanceBase<T, ATTRIBUTE_MODE_RW>(attributeName)
        {
        }
    };

    template <class T>
    class AttributeRead : AttributeInstanceBase<T, ATTRIBUTE_MODE_RO>
    {
    public:
        AttributeRead(const char *attributeName)
            : AttributeInstanceBase<T, ATTRIBUTE_MODE_RO>(attributeName)
        {
        }
    };

    template <class T>
    class AttributeWrite : AttributeInstanceBase<T, ATTRIBUTE_MODE_WO>
    {
    public:
        AttributeWrite(const char *attributeName)
            : AttributeInstanceBase<T, ATTRIBUTE_MODE_WO>(attributeName)
        {
        }
    };
} // namespace ev3api