#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

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

        string getAddress() { return m_address.getValue(Path); }
        string getDriverName() { return m_driver_name.getValue(Path); }
    };
} // namespace ev3api