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
TachoMotor motor("/sys/class/tacho-motor/motor0/");

// To re-initialize to something else
//gyro.~Sensor();
//new (&gyro) Sensor("/sys/class/lego-sensor/sensor0/");

void gyroDebug()
{
	static int m = 0;

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
}

void motorDebug()
{
	static int m = 0;
	static int s = 0;

	static const char *modes[] = {"coast", "brake", "hold"};
	static float speeds[] = {1.0f, 0.5f, 0.25f};

	if (buttons.center())
	{
		display.print("\nRun for 3s\n");
		utils.msleep(500);
		motor.setTimeSp(3000);
		motor.setCommand("run-timed");
	}

	if (buttons.up())
	{
		display.print("\nRun 3 rotations...\n");
		utils.msleep(500);
		motor.setPositionSp(motor.getCountPerRot() * 3);
		motor.setCommand("run-to-rel-pos");
	}

	if (buttons.down())
	{
		m = (m + 1) % 3;
		display.print("\nMode %i: %s\n", m, modes[m]);
		utils.msleep(500);
		motor.setStopAction(modes[m]);
	}

	if (buttons.left())
	{
		s = (s + 1) % 3;
		int speed = (int)(speeds[s] * motor.getMaxSpeed());
		display.print("\nSpeed %i: %i\n", s, speed);
		utils.msleep(500);
		motor.setSpeedSp(speed);
	}

	display.print("State: %s\n", motor.getStateString().c_str());
	display.print("Position: %i\n", motor.getPosition());
	display.print("Speed: %i / %i\n", motor.getSpeed(), motor.getMaxSpeed());
	display.print("Duty cycle: %i\n", motor.getDutyCycle());
	display.print("Speed sp: %i\n", motor.getSpeedSp());
	display.print("Time sp: %i\n", motor.getTimeSp());
	display.print("Duty cycle sp: %i\n", motor.getDutyCycleSp());
	display.print("Stop action: %s\n", motor.getStopAction().c_str());
}

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

	while (!buttons.back())
	{
		display.clear();

		//gyroDebug();
		motorDebug();

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
