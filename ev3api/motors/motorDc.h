#pragma once
#include "motor.h"

using namespace std;

namespace ev3api
{
    class DcMotor : public Motor
    {
    public:
        Attribute<string> m_commands = "commands";
        Attribute<int, SerializerTextInt> m_dutyCycle = "duty_cycle";
        Attribute<string> m_stopActions = "stop_actions";

        VariableRaw m_polarity = "polarity";
        Variable<int, SerializerTextInt> m_dutyCycleSp = "duty_cycle_sp";
        Variable<int, SerializerTextInt> m_rampUpSp = "ramp_up_sp";
        Variable<int, SerializerTextInt> m_rampDownSp = "ramp_down_sp";
        Variable<int, SerializerTextInt> m_timeSp = "time_sp";
        Variable<string> m_stopAction = "stop_action";

    public:
        DcMotor(string devicePath)
            : Motor(devicePath)
        {
        }
        
        DcMotor(OutputPort port)
            : Motor(findDevicePath(DC_MOTOR_PATH, port))
        {
        }

        // Gets available commands
        string getCommands() { return m_commands.getValue(Path); }

        // Gets motor duty cycle. -100 to 100
        int getDutyCycle() { return m_dutyCycle.getValue(Path); }

        // Gets available stop actions
        string getStopActions() { return m_stopActions.getValue(Path); }

        // Get current stop action
        string getStopAction() { return m_stopAction.getValue(Path);}

        // Sets stop action
        bool setStopAction(const char *stopAction) { return m_stopAction.setValue(Path, stopAction); }

        // Gets motor polarity, true is normal polarity and motor rotates in direction of duty cycle
        bool getPolarity() { return m_polarity.getValue<char>(Path) != 'i'; }

        // Sets motor polarity, normal polarity rotates in direction of duty cycle
        void setPolarity(bool isNormal) { m_polarity.setValue(Path, isNormal ? "normal" : "inversed"); }

        // Gets motor duty cycle set point of the PWM signal. -100, 100
        int getDutyCycleSp() { m_dutyCycleSp.getValue(Path); }

        // Sets motor duty cycle set point of the PWM signal. -100, 100
        void setDutyCycleSp(int dutyCycleSp) { m_dutyCycleSp.setValue(Path, dutyCycleSp); }

        // Gets the time in milliseconds that it take the motor to up ramp up from 0% to 100%. 0 to 10000 (10s), default 0
        int getRampUpSp() { m_rampUpSp.getValue(Path); }

        // Sets the time in milliseconds that it take the motor to up ramp up from 0% to 100%. 0 to 10000 (10s), default 0
        void setRampUpSp(int rampUpSp) { m_rampUpSp.setValue(Path, rampUpSp); }

        // Gets the time in milliseconds that it take the motor to Down ramp down from 100% to 0%. 0 to 10000 (10s), default 0
        int getRampDownSp() { m_rampDownSp.getValue(Path); }

        // Sets the time in milliseconds that it take the motor to Down ramp down from 100% to 0%. 0 to 10000 (10s), default 0
        void setRampDownSp(int rampDownSp) { m_rampDownSp.setValue(Path, rampDownSp); }

        // Gets the time setpoint used with the run-timed command. Units are in milliseconds. Values must not be negative.
        int getTimeSp() { m_timeSp.getValue(Path); }

        // Sets the time setpoint used with the run-timed command. Units are in milliseconds. Values must not be negative.
        void setTimeSp(int timeSp) { m_timeSp.setValue(Path, timeSp); }
    };
} // namespace ev3api