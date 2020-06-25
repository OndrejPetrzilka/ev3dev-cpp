#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "textUtils.h"
#include "attributeFile.h"
#include "serializer/serializer.h"
#include "serializer/serializerString.h"
#include "serializer/serializerConstCharPtr.h"

namespace ev3api
{
    extern constexpr char ATTRIBUTE_MODE_RW[] = "r+";
    extern constexpr char ATTRIBUTE_MODE_RO[] = "r";
    extern constexpr char ATTRIBUTE_MODE_WO[] = "w";

    template <const char *openMode>
    class AttributeBase : public AttributeFile
    {
    protected:
        typedef AttributeBase<openMode> base;

        template <class T, class TSerializer = Serializer<T>>
        int read(const char *devicePath, T *buffer, int count = 1, int byteOffset = 0)
        {
            if (open(devicePath))
            {
                fseek(getHandle(), byteOffset, SEEK_SET);
                return TSerializer::read(getHandle(), buffer, count);
            }
            return -1;
        }

        // Reads line from attribute into buffer, null-terminates the string, does not include line break
        int readLine(const char *devicePath, char *buffer, int bufferSize, int byteOffset = 0)
        {
            if (open(devicePath))
            {
                fseek(getHandle(), byteOffset, SEEK_SET);
                int read = fread(buffer, sizeof(char), bufferSize, getHandle());
                read = findBreak(buffer, read);
                buffer[read >= bufferSize ? bufferSize - 1 : read] = '\0';
                return read;
            }
            return -1;
        }

        template <class T, class TSerializer = Serializer<T>>
        int write(const char *devicePath, const T *buffer, int count = 1, int byteOffset = 0)
        {
            if (open(devicePath))
            {
                fseek(getHandle(), byteOffset, SEEK_SET);
                return TSerializer::write(getHandle(), buffer, count);
            }
            return -1;
        }

        template <class T, class TSerializer = Serializer<T>>
        T getValue(const char *devicePath, int byteOffset = 0)
        {
            T result;
            return read<T, TSerializer>(devicePath, &result, 1, byteOffset) > 0 ? result : T();
        }

        template <class T, class TSerializer = Serializer<T>>
        bool setValue(const char *devicePath, const T &value, int byteOffset = 0)
        {
            return write<T, TSerializer>(devicePath, &value, 1, byteOffset) > 0;
        }

    public:
        const char *const AttributeName;

        AttributeBase(const char *attributeName)
            : AttributeName(attributeName)
        {
        }

        const char *getAttributeMode() const
        {
            return openMode;
        }

        bool open(const char *devicePath)
        {
            return AttributeFile::open(devicePath, AttributeName, openMode);
        }
    };

    class AttributeRaw : public AttributeBase<ATTRIBUTE_MODE_RO>
    {
    public:
        using base::base;
        using base::getValue;
        using base::read;
        using base::readLine;
    };

    class VariableRaw : public AttributeBase<ATTRIBUTE_MODE_RW>
    {
    public:
        using base::base;
        using base::getValue;
        using base::read;
        using base::readLine;
        using base::setValue;
        using base::write;
    };

    template <class T, class TSerializer = Serializer<T>>
    class Variable : public AttributeBase<ATTRIBUTE_MODE_RW>
    {
    public:
        using base::base;

        const VariableRaw& asRaw() { return *(VariableRaw*)this; }

        int read(const char *devicePath, T *buffer, int count = 1) { return base::read<T, TSerializer>(devicePath, buffer, count); }
        int write(const char *devicePath, const T *buffer, int count = 1) { return base::write<T, TSerializer>(devicePath, buffer, count); }

        T getValue(const char *devicePath) { return base::getValue<T, TSerializer>(devicePath); }
        bool setValue(const char *devicePath, const T &value) { return base::setValue<T, TSerializer>(devicePath, value); }
    };

    template <class T, class TSerializer = Serializer<T>>
    class Attribute : public AttributeBase<ATTRIBUTE_MODE_RO>
    {
    public:
        using base::base;

        const AttributeRaw& asRaw() { return *(AttributeRaw*)this; }

        int read(const char *devicePath, T *buffer, int count = 1) { return base::read<T, TSerializer>(devicePath, buffer, count); }
        T getValue(const char *devicePath) { return base::getValue<T, TSerializer>(devicePath); }
    };

    template <class T, class TSerializer = Serializer<T>>
    class Command : public AttributeBase<ATTRIBUTE_MODE_WO>
    {
    public:
        using base::base;

        int write(const char *devicePath, const T *buffer, int count = 1) { return base::write<T, TSerializer>(devicePath, buffer, count); }
        bool setValue(const char *devicePath, const T &value) { return base::setValue<T, TSerializer>(devicePath, value); }
    };
} // namespace ev3api