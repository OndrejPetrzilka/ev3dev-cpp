#pragma once
#include "motor.h"

using namespace std;

namespace ev3api
{
    class ServoMotor : public Motor
    {
    public:
    public:
        ServoMotor(string devicePath)
            : Motor(devicePath)
        {
        }

        ServoMotor(OutputPort port)
            : Motor(findDevicePath(SERVO_MOTOR_PATH, port))
        {
        }
    };
} // namespace ev3api