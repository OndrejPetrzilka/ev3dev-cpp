#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "display.h"
#include "debug.h"
#include "buttons.h"
#include "leds.h"
#include "sensors/touch.h"
#include "sensors/gyro.h"
#include "motors/motorTacho.h"

using namespace ev3api;

EV3Buttons buttons;
EV3Led led(LED_LEFT_GREEN);

Touch touch("/sys/class/lego-sensor/sensor1/");
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

int main(int argc, const char *argv[])
{
	display.setFont("Uni2-VGA16");
	
	while (!buttons.back())
	{
		display.clear();
		display.print("\nIsPressed: %i\n", touch.isPressed() ? 1 : 0);
		utils.msleep(200);
	}
	return 0;
}
