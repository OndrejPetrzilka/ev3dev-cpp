#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "display.h"
#include "debug.h"
#include "buttons.h"
#include "leds.h"
#include "sensors/gyro.h"
#include "motors/motorTacho.h"

using namespace ev3api;

EV3Buttons buttons;
EV3Led led(LED_LEFT_GREEN);

//  sys\class\lego-sensor
Gyro gyro("/sys/class/lego-sensor/sensor0/");

// To re-initialize to something else
//gyro.~Sensor();
//new (&gyro) Sensor("/sys/class/lego-sensor/sensor0/");

int main(int argc, const char *argv[])
{
	int x = 1;

	display.setFont("Uni2-VGA16");
	display.print("Hello World! %i\n", x++);
	display.print("Hello World! %i\n", x++);
	display.print("Hello World! %i\n", x++);
	display.print("Hello World! %i\n", x++);
	display.print("Hello World! %i\n", x++);
	display.print("Hello World! %i\n", x++);
	display.print("Hello World! %i\n", x++);

	display.clear();
	display.print("Hello World! %i\n", x++);

	debug.print("This is stderr new test %i\n", x);

	debug.print("buttons.getKeys: %i\n", buttons.getKeys());

	gyro.setMode(Gyro::Mode::GYRO_ANG);

	int m = 0;

	while (!buttons.back())
	{
		display.clear();
		if (buttons.center())

		{
			display.print("resetting...\n");
			utils.msleep(2000);
			gyro.reset();
		}

		if (buttons.up())
		{
			display.print("calibrating...\n");
			utils.msleep(2000);
			gyro.calibrate();
		}

		if (buttons.right())
		{
			m = (m + 1) % 7;
			display.print("Set mode %i\n", m);
			gyro.setMode((Gyro::Mode)m);
			utils.msleep(1000);
		}

		if (buttons.left())
		{
			m = (m - 1 + 7) % 7;
			display.print("Set mode %i\n", m);
			gyro.setMode((Gyro::Mode)m);
			utils.msleep(1000);
		}

		display.print("%s\n", gyro.getModeString());
		display.print("%s\n", gyro.getSensor().getMode().c_str());
		display.print("Mode: %i\n", (int)gyro.getMode());
		display.print("Vals: %i, Decs: %i\n", gyro.getNumValues(), gyro.getDecimals());
		display.print("Binary: %i %s\n", gyro.getSensor().getBinData<short>(), gyro.getUnits().c_str());
		display.print("Debug: %s\n", gyro.getSensor().getDebugValues().c_str());

		//display.clear();
		//display.print("A,UDLFC %i, %i %i %i %i %i\n", buttons.getKeys(), buttons.up(), buttons.down(), buttons.left(), buttons.right(), buttons.center());
		//debug.print("A,UDLFC %i, %i %i %i %i %i\n", buttons.getKeys(), buttons.up(), buttons.down(), buttons.left(), buttons.right(), buttons.center());
		utils.msleep(200);
	}

	//system("beep");
	//system("beep");
	//sleep(2);
	return 0;
}
