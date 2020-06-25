#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

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

Sensor sensor("/sys/class/lego-sensor/sensor4/");

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

	string buf;
	stringstream ss(sensor.getModes());
	vector<string> tokens;

	while (ss >> buf)
		tokens.push_back(buf);

	int modeCount = tokens.size();

	while (!buttons.back())
	{
		display.clear();
		display.print("\n");

		static int m = 0;

		if (buttons.right())
		{
			m = (m + 1) % modeCount;
			display.print("Mode %i: %s\n", m, tokens[m]);
			sensor.setMode(tokens[m].c_str());
			utils.msleep(1000);
		}
		if (buttons.left())
		{
			m = (m - 1 + modeCount) % modeCount;
			display.print("Mode %i: %s\n", m, tokens[m]);
			sensor.setMode(tokens[m].c_str());
			utils.msleep(1000);
		}

		string mode = sensor.getMode();
		vector<string>::iterator it = find(tokens.begin(), tokens.end(), mode);
		int index = distance(tokens.begin(), it);

		display.print("Mode: %i / %i\n", index, tokens.size());
		display.print("Mode: %s\n", mode.c_str());
		display.print("Vals: %i, Decs: %i\n", sensor.getNumValues(), sensor.getDecimals());
		display.print("Debug: %s\n", sensor.getDebugValues().c_str());

		utils.msleep(200);
	}
	return 0;
}
