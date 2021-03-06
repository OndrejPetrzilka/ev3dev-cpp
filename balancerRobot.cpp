// C++ rewrite of balancer-basic ev3 project
// http://robotsquare.com/2014/07/01/tutorial-ev3-self-balancing-robot/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "mathUtils.h"
#include "timeUtil.h"
#include "display.h"
#include "debug.h"
#include "buttons.h"
#include "leds.h"
#include "sensors/gyro.h"
#include "motors/motorTacho.h"
#include "reporting.h"

using namespace ev3api;

#define CLEAR_SCREEN "\033[H\033[J"

EV3Buttons buttons;

Gyro gyro(InputPort::InputPort2);
Sensor us(InputPort::InputPort3);
TachoMotor motorRight(OutputPort::OutputPortA);
TachoMotor motorLeft(OutputPort::OutputPortD);

// configuration constants
const float gainMotorPosition = 350;
const float gainMotorSpeed = 75;
const float gainAngle = 25;
const float gainAngularVelocity = 1.3f;
const float kd = 0.005f;
const float ki = 14;
const float kp = 0.6f;
const int maxIndex = 7;

// parameters constants
const float wheelDiameter = 42;
const float sampleTime = 22;

// derived constants
float dt = (sampleTime - 2) / 1000;
float radius = wheelDiameter / 2000;

// variables
float gyroType = 0;
float encArray[maxIndex] = {};
float refPos = 0;
bool nowOutOfBound = false;
bool prevOutOfBound = false;
float outOfBoundCount = 0;
float outOfBound = 0;
float ang = 0;
float mean = 0;
float meanAng = 0;
float speed = 0;
float steering = 0;
float maxAcceleration = 0;

float leftTachoCountPerRotation;
float rightTachoCountPerRotation;

float robotPos = 0;
float robotSpeed = 0;

float angle = 0;
float angularVelocity = 0;
float steer = 0;
float input = 0;
float output = 0;

float prevErr = 0;
int encIndex = 0;

uint64_t lastDisplay = 0;

float gyroRate()
{
    return gyro.getRawValue<short>() / -4.0f;
}

void getPosition(float &refPos)
{
    refPos = refPos + (dt * speed * 0.002);
}

float getMotorSpeed()
{
    // This seems like average motor speed over last 7 frames
    encIndex++;
    if (encIndex >= maxIndex)
    {
        encIndex = 0;
    }

    int compareIndex = encIndex + 1;
    if (compareIndex >= maxIndex)
    {
        compareIndex = 0;
    }

    float leftAngle = motorLeft.getPosition() / leftTachoCountPerRotation * 360.0f;
    float rightAngle = motorRight.getPosition() / rightTachoCountPerRotation * 360.0f;

    reportData("MotorAngL", leftAngle);
    reportData("MotorAngR", rightAngle);

    encArray[encIndex] = (leftAngle + rightAngle) / 2;

    return (encArray[encIndex] - encArray[compareIndex]) / (dt * maxIndex);
}

void readEncoders(float &robotPos, float &robotSpeed)
{
    float leftMotorPos = motorLeft.getPosition() / leftTachoCountPerRotation * 360.0f;
    float rightMotorPos = motorRight.getPosition() / rightTachoCountPerRotation * 360.0f;

    robotSpeed = (radius * getMotorSpeed()) / 57.3f;
    robotPos = (leftMotorPos + rightMotorPos) / 2 * radius / 57.3f;
}

void readGyro(float &angle, float &angularVelocity)
{
    float currValue = gyroRate();

    float a = mean;
    float b = currValue;
    float c = dt * 0.2f;

    mean = (a * (1 - c)) + (b * c);

    angularVelocity = currValue - mean;
    angle = angle + angularVelocity * dt;
}

float combineSensorValues(float angularVelocity, float angle, float motorSpeed, float motorPos, float refPos)
{
    float a = gainMotorPosition * (motorPos - refPos);
    float b = gainMotorSpeed * motorSpeed;
    float c = gainAngle * angle;
    float d = gainAngularVelocity * angularVelocity;

    reportData("_i MotorPos", a);
    reportData("_i MotorSpeed", b);
    reportData("_i Angle", c);
    reportData("_i AngVelocity", d);

    return a + b + c + d;
}

float pid(float input)
{
    float referenceValue = 0;
    float currErr = input - referenceValue;

    float accErr = accErr + (currErr * dt);

    float difErr = (currErr - prevErr) / dt;

    float c = currErr * kp;
    float b = accErr * ki;
    float a = difErr * kd;

    reportData("_err Acc", accErr);
    reportData("_err Curr", currErr);
    reportData("_err Dif", difErr);

    reportData("_errs Acc", accErr);
    reportData("_errs Curr", currErr);
    reportData("_errs Dif", difErr);

    return a + b + c;
}

bool handleErrors(float output)
{
    if (abs(output) > 100)
    {
        nowOutOfBound = true;
    }

    if (nowOutOfBound && prevOutOfBound)
    {
        outOfBoundCount++;
    }
    else
    {
        outOfBoundCount = 0;
    }

    if (outOfBoundCount > 20)
    {
        return false;
    }
    prevOutOfBound = nowOutOfBound;
    return true;
}

float getSteer()
{
    return steering;
}

float setMotorPower(float steer, float avgPower)
{
    steer = clamp(steer, -50, 50);

    // TODO: Steer
    float extraPower = 0;

    float pwrLeft = (avgPower + extraPower) * 0.021f / radius;
    float pwrRight = (avgPower - extraPower) * 0.021f / radius;

    reportData("_o PowerL", pwrLeft);
    reportData("_o PowerR", pwrRight);

    motorLeft.setDutyCycleSp(round(pwrLeft));
    motorRight.setDutyCycleSp(round(pwrRight));
}

void initialize()
{
    gyro.setMode(Gyro::Mode::GYRO_RATE);

    motorLeft.setCommand("reset");
    motorRight.setCommand("reset");

    motorLeft.setSpeedSp(motorLeft.getMaxSpeed());
    motorRight.setSpeedSp(motorRight.getMaxSpeed());

    motorLeft.setCommand("run-direct");
    motorRight.setCommand("run-direct");

    gyroType = 0;
    encArray[maxIndex] = {};
    refPos = 0;
    nowOutOfBound = false;
    prevOutOfBound = false;
    outOfBoundCount = 0;
    outOfBound = 0;
    ang = 0;
    mean = 0;
    meanAng = 0;
    speed = 0;
    steering = 0;
    maxAcceleration = 0;

    leftTachoCountPerRotation = motorLeft.getCountPerRot();
    rightTachoCountPerRotation = motorRight.getCountPerRot();

    refPos = 0;
    robotPos = 0;
    robotSpeed = 0;

    angle = 0;
    angularVelocity = 0;
    steer = 0;
    input = 0;
    output = 0;

    reportData("* TachoPerRotL", leftTachoCountPerRotation);
    reportData("* TachoPerRotR", rightTachoCountPerRotation);
    reportData("* Radius", radius);
    reportData("* dt", dt);

    utils.msleep(100);

    //system("beep");
    utils.msleep(100);
    mean = 0;
    for (int i = 0; i < 20; i++)
    {
        mean += gyroRate();
        utils.msleep(5);
    }
    mean /= 20.0f;

    utils.msleep(100);
    //system("beep");
    utils.msleep(100);
    //system("beep");
}

void updateDisplay(float angle, float angularVelocity, float output)
{
    if (elapsedMs(lastDisplay) > 250)
    {
        lastDisplay = nowMs();

        const char *formatStr =
            CLEAR_SCREEN
            "[x] On/Off, [>] Off\n"
            "Gyro: %.2f, %.2f °/s\n"
            "Speed: %i, %i\n"
            "Position: %i, %i\n"
            "Output: %.2f\n";
        display.print(formatStr, angle, angularVelocity, motorLeft.getSpeed(), motorRight.getSpeed(), motorLeft.getPosition(), motorRight.getPosition(), output);
    }
}

void setRunning(bool isRunning)
{
    if (isRunning)
    {
        initialize();
    }
    else
    {
        motorLeft.setCommand("reset");
        motorRight.setCommand("reset");
    }
}

void handleInput(bool &running)
{
    if (buttons.center())
    {
        running = !running;
        utils.msleep(500);
        setRunning(running);
    }
    if (buttons.right())
    {
        running = false;
        utils.msleep(500);
        setRunning(false);
    }
    if (buttons.up())
    {
        running = true;
        utils.msleep(500);
        setRunning(true);
    }
}

int main(int argc, const char *argv[])
{
    display.setFont("Uni2-VGA16");

    bool running = false;
    running = true;
    setRunning(true);

    Time base = Time::now();
    Time timeWindow = Time::now();
    int frameIndex = 0;
    int frameWindow = 0;

    while (true)
    {
        Time start = Time::now();

        handleInput(running);

        if (running)
        {
            getPosition(refPos);
            readEncoders(robotPos, robotSpeed);
            readGyro(angle, angularVelocity);

            reportData("RefPos", refPos);
            reportData("RobotPos", robotPos);
            reportData("RobotSpeed", robotSpeed);
            reportData("Angle", angle);
            reportData("AngVelocity", angularVelocity);

            input = combineSensorValues(angularVelocity, angle, robotSpeed, robotPos, refPos);

            reportData("_i", input);

            output = pid(input);

            reportData("_o", output);

            if (!handleErrors(output))
            {
                //debug.printLine("Error, out of bounds");
                //return 1;
                reportData("Error", 1);
            }
            else
            {
                reportData("Error", 0);
            }

            steer = getSteer();
            setMotorPower(steer, output);
        }
        updateDisplay(angle, angularVelocity, output);

        frameIndex++;
        frameWindow++;
        reportData("_t Frame", frameIndex);
        reportData("_t Time ms", (int)base.elapsedMiliseconds());

        if (timeWindow.elapsedSeconds() > 0.5)
        {
            reportData("_t FPS", frameWindow / timeWindow.elapsedSeconds());
            timeWindow = Time::now();
            frameWindow = 0;
        }

        auto remaining = dt * 1000.0 - start.elapsedMiliseconds();
        if (remaining > 0)
        {
            // Sleep except for last milisecond
            if (remaining > 2)
                utils.msleep(remaining - 1);

            while (start.elapsedSeconds() < dt)
            {
                // Active waiting last milisecond
            }
        }
    }
}
