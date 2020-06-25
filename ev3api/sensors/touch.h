#pragma once
#include "sensor.h"
#include "utils.h"
#include <array>

namespace ev3api
{
    class Touch : Sensor
    {
    public:
        Attribute<int, SerializerTextInt> m_value0 = "value0";

        Touch(const char *devicePath)
            : Sensor(devicePath)
        {
        }

        Sensor &getSensor()
        {
            return *this;
        }

        bool isPressed()
        {
            return m_value0.getValue(Path) > 0;
        }
    };
} // namespace ev3api