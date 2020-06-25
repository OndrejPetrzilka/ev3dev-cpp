#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "attribute.h"

namespace ev3api
{
    // Base-class for sysfs device
    class Device
    {
    public:
        const char *const Path;

        Device(const char *devicePath)
            : Path(devicePath)
        {
        }

        ~Device()
        {
        }
    };
} // namespace ev3api