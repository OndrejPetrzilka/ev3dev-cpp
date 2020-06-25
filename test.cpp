#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ev3api/display.h"
#include "ev3api/debug.h"
#include "ev3api/buttons.h"
#include "ev3api/leds.h"
#include "ev3api/sensor.h"

using namespace ev3api;

EV3Buttons buttons;
EV3Led led(LED_LEFT_GREEN);

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

	while (!buttons.back())
	{
		//display.clear();
		//display.print("A,UDLFC %i, %i %i %i %i %i\n", buttons.getKeys(), buttons.up(), buttons.down(), buttons.left(), buttons.right(), buttons.center());
		//debug.print("A,UDLFC %i, %i %i %i %i %i\n", buttons.getKeys(), buttons.up(), buttons.down(), buttons.left(), buttons.right(), buttons.center());
		usleep(200000);
	}

	//system("beep");
	//system("beep");
	//sleep(2);
	return 0;
}
