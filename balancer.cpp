#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "mathUtils.h"
#include "display.h"
#include "debug.h"
#include "buttons.h"
#include "leds.h"
#include "sensors/gyro.h"
#include "motors/motorTacho.h"

using namespace ev3api;

#define CLEAR_SCREEN "\033[H\033[J"

EV3Buttons buttons;
EV3Led led(LED_LEFT_GREEN);

Gyro gyro(InputPort::InputPort2);
Sensor us(InputPort::InputPort3);
TachoMotor motorRight(OutputPort::OutputPortA);
TachoMotor motorLeft(OutputPort::OutputPortD);

// CONFIGURATION

// Wheel radius in mm
const float WheelRadius = 55;

// Center of mass height in mm
const float CenterOfMassHeight = 170;

// UltraSound sensor height in mm for balanced robot
const float BalanceSensorDist = 219;

void drive(TachoMotor &left, TachoMotor &right, bool apply, int angleRate, int distance, int &outLeft, int &outRight)
{
	// Absolute gyro angle is unreliable, use rate instead

	int duty = std::roundf(clamp(angleRate * 10.0f, -100.0f, 100.0f));
	//int duty =  rate > 0 ? 10 : (rate < 0 ? -10 : 0);

	outLeft = duty;
	outRight = duty;

	if (apply)
	{
		left.setDutyCycleSp(outLeft);
		right.setDutyCycleSp(outRight);
	}
}

void setRunning(TachoMotor &motor, bool running)
{
	if (running)
	{
		motor.setStopAction("coast");
		motor.setSpeedSp(motor.getMaxSpeed());
		motor.setCommand("run-direct");
	}
	else
	{
		motor.setCommand("reset");
	}
}

void setRunning(bool running)
{
	setRunning(motorLeft, running);
	setRunning(motorRight, running);
}

int main(int argc, const char *argv[])
{
	display.setFont("Uni2-VGA16");
	gyro.setMode(Gyro::Mode::GYRO_RATE);

	us.setMode("US-DIST-CM");

	setRunning(false);

	uint64_t time = 0;

	bool running = false;
	bool enableDisplay = true;

	debug.print("test");

	while (!buttons.back())
	{
		if (buttons.center())
		{
			running = !running;
			utils.msleep(500);

			setRunning(running);
			enableDisplay = running;
		}
		if (buttons.down())
		{
			display.clear();
			display.print("\n\nCalibrating...");
			utils.msleep(2000);
			gyro.calibrate();
		}
		if (buttons.up())
		{
			display.print("\n\nResetting...");
			utils.msleep(2000);
			gyro.reset();
			utils.msleep(500);
		}
		if (buttons.right())
		{
			running = false;
			utils.msleep(500);
			setRunning(false);
		}
		if (buttons.left())
		{
			enableDisplay = true;
		}

		if (!running)
		{
			enableDisplay = true;
		}

		short rate = gyro.getRawValue<short>();

		int distMm = us.getBinData<short>();

		int driveLeft, driveRight;
		drive(motorLeft, motorRight, running, rate, distMm, driveLeft, driveRight);

		const char *formatStr =
			CLEAR_SCREEN
			"[x] On/Off, [v] Cal\n"
			"[^] Reset, [>] Off\n"
			"Dist: %i\n"
			"Gyro: %i °/s\n"
			"Speed: %i, %i\n"
			"Position: %i, %i\n"
			"Debug: %i, %i\n";

		// Update UI few times per second
		if (enableDisplay && elapsedMs(time) > 250)
		{
			time = nowMs();
			display.print(formatStr, distMm, rate, motorLeft.getSpeed(), motorRight.getSpeed(), motorLeft.getPosition(), motorRight.getPosition(), driveLeft, driveRight);
		}
	}
	return 0;
}
