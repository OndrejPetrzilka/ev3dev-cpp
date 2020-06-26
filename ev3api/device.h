#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>

#include "attribute/attribute.h"
#include "serializer/serializer.h"

using namespace std;

namespace ev3api
{
    // Base-class for sysfs device
    class Device
    {
    public:
        const char *const Path;
        
        Attribute<string> m_address = "address";
        Attribute<string> m_driver_name = "driver_name";

        Device(const char *devicePath)
            : Path(devicePath)
        {
        }

        const char *getPath() const
        {
            return Path;
        }

        bool isConnected()
        {
            struct stat sb;
            return stat(Path, &sb) == 0 && S_ISDIR(sb.st_mode);
        }

        string getAddress() { return m_address.getValue(Path); }
        string getDriverName() { return m_driver_name.getValue(Path); }
    };
} // namespace ev3api