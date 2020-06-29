#pragma once

enum class InputPort
{
    InputPort1 = 0,
    InputPort2 = 1,
    InputPort3 = 2,
    InputPort4 = 3,
};

enum class OutputPort
{
    OutputPortA = 0,
    OutputPortB = 1,
    OutputPortC = 2,
    OutputPortD = 3,
};

extern constexpr const char *const INPUT_PORTS[] = {"ev3-ports:in1", "ev3-ports:in2", "ev3-ports:in3", "ev3-ports:in4"};
extern constexpr const char *const OUTPUT_PORTS[] = {"ev3-ports:outA", "ev3-ports:outB", "ev3-ports:outC", "ev3-ports:outD"};

extern constexpr const char *SENSOR_PATH = "/sys/class/lego-sensor/";
extern constexpr const char *TACHO_MOTOR_PATH = "/sys/class/tacho-motor/";
extern constexpr const char *DC_MOTOR_PATH = "/sys/class/dc-motor/";
extern constexpr const char *SERVO_MOTOR_PATH = "/sys/class/servo-motor/";
