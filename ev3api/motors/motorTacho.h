#pragma once
#include "motorDc.h"

using namespace std;

namespace ev3api
{
    // EV3 Lego tacho motor, reports position at about 500 Hz
    class TachoMotor : public DcMotor
    {
    public:
        Attribute<string> m_commands = "commands";
        Attribute<int, SerializerTextInt> m_countPerRot = "count_per_rot";
        Attribute<int, SerializerTextInt> m_countPerM = "count_per_m";
        Attribute<int, SerializerTextInt> m_fullTravelCount = "full_travel_count";
        Attribute<int, SerializerTextInt> m_maxSpeed = "max_speed";
        Attribute<int, SerializerTextInt> m_speed = "speed";

        Variable<int, SerializerTextInt> m_position = "position";
        Variable<int, SerializerTextInt> m_positionSp = "position_sp";
        Variable<int, SerializerTextInt> m_speedSp = "speed_sp";
        Variable<int, SerializerTextInt> m_holdPidKd = "hold_pid/Kd";
        Variable<int, SerializerTextInt> m_holdPidKi = "hold_pid/Ki";
        Variable<int, SerializerTextInt> m_holdPidKp = "hold_pid/Kp";
        Variable<int, SerializerTextInt> m_speedPidKd = "speed_pid/Kd";
        Variable<int, SerializerTextInt> m_speedPidKi = "speed_pid/Ki";
        Variable<int, SerializerTextInt> m_speedPidKp = "speed_pid/Kp";

    public:
        TachoMotor(const char *devicePath)
            : DcMotor(devicePath)
        {
        }

        // Returns the number of tacho counts in one rotation of the motor. Tacho counts are used by the position and speed attributes, so you can use this value to convert from rotations or degrees to tacho counts. (rotation motors only)
        int getCountPerRot() { return m_countPerRot.getValue(Path); }

        // Returns the number of tacho counts in one meter of travel of the motor. Tacho counts are used by the position and speed attributes, so you can use this value to convert from distance to tacho counts. (linear motors only)
        int getCountPerM() { return m_countPerM.getValue(Path); }

        // Returns the number of tacho counts in the full travel of the motor. When combined with the count_per_m atribute, you can use this value to calculate the maximum travel distance of the motor. (linear motors only)
        int getFullTravelCount() { return m_fullTravelCount.getValue(Path); }

        // Returns the maximum value that is accepted by the speed_sp attribute. This value is the speed of the motor at 9V with no load. Note: The actual maximum obtainable speed will be less than this and will depend on battery voltage and mechanical load on the motor.
        int getMaxSpeed() { return m_maxSpeed.getValue(Path); }

        // Returns the current motor speed in tacho counts per second. Note, this is not necessarily degrees (although it is for LEGO motors). Use the count_per_rot attribute to convert this value to RPM or deg/sec.
        int getSpeed() { return m_speed.getValue(Path); }

        // Returns the current position of the motor in pulses of the rotary encoder.
        // When the motor rotates clockwise, the position will increase. Likewise, rotating counter-clockwise causes the position to decrease. 
        // The range is -2,147,483,648 and +2,147,483,647 tachometer counts (32-bit signed integer).
        int getPosition() { return m_position.getValue(Path); }

        // Sets current position of the motor in pulses of the rotary encoder.
        // The range is -2,147,483,648 and +2,147,483,647 tachometer counts (32-bit signed integer).
        void setPosition(int value) { m_position.setValue(Path, value); }

        // Gets target position for the run-to-abs-pos and run-to-rel-pos commands.
        // Units are in tacho counts. 
        // You can use the value returned by counts_per_rot to convert tacho counts to/from rotations or degrees. 
        // The range is -2,147,483,648 and +2,147,483,647 tachometer counts (32-bit signed integer).
        int getPositionSp() { return m_positionSp.getValue(Path); }

        // Sets target position for the run-to-abs-pos and run-to-rel-pos commands.
        // Units are in tacho counts. 
        // You can use the value returned by counts_per_rot to convert tacho counts to/from rotations or degrees. 
        // The range is -2,147,483,648 and +2,147,483,647 tachometer counts (32-bit signed integer).
        void setPositionSp(int value) { m_positionSp.setValue(Path, value); }

        // Gets the target speed in tacho counts per second used for all run-* commands except run-direct.
        // A negative value causes the motor to rotate in reverse with the exception of run-to-*-pos commands where the sign is ignored.
        // Use the count_per_rot attribute to convert RPM or deg/sec to tacho counts per second.
        // Use the count_per_m attribute to convert m/s to tacho counts per second.
        int getSpeedSp() { return m_speedSp.getValue(Path); }

        // Sets the target speed in tacho counts per second used for all run-* commands except run-direct.
        // A negative value causes the motor to rotate in reverse with the exception of run-to-*-pos commands where the sign is ignored.
        // Use the count_per_rot attribute to convert RPM or deg/sec to tacho counts per second.
        // Use the count_per_m attribute to convert m/s to tacho counts per second.
        void setSpeedSp(int value) { m_speedSp.setValue(Path, value); }
    };
} // namespace ev3api