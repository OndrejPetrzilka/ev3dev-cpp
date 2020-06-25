#pragma once
#include "motor.h"

using namespace std;

namespace ev3api
{
    class ServoMotor : public Motor
    {
    public:

    public:
        ServoMotor(const char *devicePath)
            : Motor(devicePath)
        {
        }
    };
} // namespace ev3api