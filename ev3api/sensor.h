#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "device.h"
#include "attributeInstance.h"

namespace ev3api
{
    // Base-class for sensor
    class Sensor : Device
    {
    protected:
        AttributeRead<int> m_numValues = "numValues";

    public:
        Sensor(const char *devicePath)
            : Device(devicePath)
        {
        }
    };
} // namespace ev3api