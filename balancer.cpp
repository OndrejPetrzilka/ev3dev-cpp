#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "display.h"
#include "debug.h"
#include "buttons.h"
#include "leds.h"
#include "sensors/gyro.h"
#include "motors/motorTacho.h"

using namespace ev3api;

EV3Buttons buttons;
EV3Led led(LED_LEFT_GREEN);

Gyro gyro("/sys/class/lego-sensor/sensor0/");
TachoMotor motorRight("/sys/class/tacho-motor/motor0/");
TachoMotor motorLeft("/sys/class/tacho-motor/motor1/");

const float restAngle = 21;

uint64_t nowMs()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL; // calculate milliseconds
}

// Gets time elapsed since timestamp
uint64_t elapsedMs(uint64_t timestamp)
{
	return nowMs() - timestamp;
}

float clamp(float value, float min, float max)
{
	return value < min ? min : (value > max ? max : value);
}

inline float deg2rad(float degrees)
{
	return degrees / (180.0 / M_PI);
}

inline float rad2deg(float radians)
{
	return radians * (180.0 / M_PI);
}

void setRunning(TachoMotor &motor, bool running)
{
	if (running)
	{
		motor.setStopAction("hold");
		motor.setSpeedSp(motor.getMaxSpeed());
		//motor.setCommand("run-direct");
	}
	else
	{
		motor.setCommand("reset");
	}
}

void drive(bool apply, int angle, int rate, int &debug)
{
	// 22 degrees tilt can be recovered by relative motor rotation of -140 units

	// Height in wheel travel units
	const float height = 374;
	const float centerOfMass = height * 0.5f;

	// How long does it take for controller to react
	const float predictionTime = 0.01f;

	float predictedAngle = angle + rate * predictionTime;

	float moveDist = cosf(deg2rad(90.0f - predictedAngle)) * centerOfMass;
	debug = -moveDist;

	float duty = roundf(clamp(-moveDist * 35.0f, -100.0f, 100.0f));

	if (apply)
	{
		//motorLeft.setDutyCycleSp(duty);
		//motorRight.setDutyCycleSp(duty);

		motorLeft.setPositionSp(motorLeft.getPosition() + roundf(-moveDist));
		motorRight.setPositionSp(motorRight.getPosition() + roundf(-moveDist));

		motorLeft.setCommand("run-to-abs-pos");
		motorRight.setCommand("run-to-abs-pos");

		// motorLeft.setCommand("run-to-rel-pos");
		// motorRight.setCommand("run-to-rel-pos");
	}

	//int duty = std::roundf(clamp(-angle * 35.0f, -100.0f, 100.0f));
	//motor.setDutyCycleSp(duty);
}

void setRunning(bool running)
{
	setRunning(motorLeft, running);
	setRunning(motorRight, running);
}

int main(int argc, const char *argv[])
{
	display.setFont("Uni2-VGA16");
	gyro.setMode(Gyro::Mode::GYRO_GA);

	setRunning(false);

	uint64_t time = 0;

	bool running = false;
	bool enableDisplay = true;
	short angleRate[2];
	int debugValue = 0;

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

		gyro.getRawValues(angleRate, 2);
		angleRate[0] += restAngle;

		drive(running, angleRate[0], angleRate[1], debugValue);

		const char *formatStr =
			"\033[H\033[J" // Clear
			"[x] On/Off, [v] Cal\n"
			"[^] Reset, [>] Off\n"
			"State: %s\n"
			"Angle: %i, Rate: %i\n"
			"Speed: %i, %i\n"
			"Position: %i, %i\n"
			"Debug: %i\n";

		// Update UI few times per second
		if (enableDisplay && elapsedMs(time) > 250)
		{
			time = nowMs();
			display.print(formatStr, motorLeft.getStateString().c_str(), angleRate[0], angleRate[1], motorLeft.getSpeed(), motorRight.getSpeed(), motorLeft.getPosition(), motorRight.getPosition(), debugValue);
		}
	}
	return 0;
}
