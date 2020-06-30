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

int main(int argc, const char *argv[])
{
	display.setFont("Uni2-VGA16");

	int x = 0;
	int y = 0;

	while (!buttons.back())
	{
		if (buttons.up())
		{
			x++;
		}
		if (buttons.down())
		{
			x--;
		}

		y++;

		reportData("xx", x);
		reportData("yy", y);

		display.clear();
		display.print("\nx: %i\ny: %i\n", x, y);
		utils.msleep(200);
	}
	return 0;
}
