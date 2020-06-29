#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <dirent.h>

#include "display.h"
#include "debug.h"
#include "buttons.h"
#include "leds.h"
#include "sensors/touch.h"
#include "sensors/gyro.h"
#include "motors/motorTacho.h"

using namespace ev3api;

const char *basePath = "/sys/class/lego-sensor/";

EV3Buttons buttons;
EV3Led led(LED_LEFT_GREEN);

Sensor sensor("/sys/class/lego-sensor/sensor5/");

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

void debugString(const char *str)
{
	// Print two lines: string at top, hex at bottom for each char
	std::stringstream ss;
	for (const char *ptr = str; *ptr != '\0'; ptr++)
	{
		ss << *ptr << " ";
	}
	ss << endl;
	for (const char *ptr = str; *ptr != '\0'; ptr++)
	{
		ss << hex << (unsigned int)*ptr;
	}
	ss << endl;
	printf(ss.str().c_str());
}

inline std::string &rtrim(std::string &s, const char *t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

vector<string> getSensors()
{
	DIR *d = opendir(basePath);
	struct dirent *dir;
	vector<string> result;
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_name[0] != '.')
			{
				result.push_back(dir->d_name);
			}
		}
		closedir(d);
	}
	return result;
}

string getSensorName(const char *sensorPath)
{
	string path(sensorPath);
	rtrim(path, "/");
	int lastSep = path.find_last_of('/');
	return path.substr(lastSep + 1);
}

string findNextSensor(const char *sensorPath, int offset)
{
	string sensorName = getSensorName(sensorPath);

	vector<string> sensors = getSensors();
	vector<string>::iterator it = find(sensors.begin(), sensors.end(), sensorName);
	if (it != sensors.end())
	{
		int index = distance(sensors.begin(), it);
		index = (index + offset + sensors.size()) % sensors.size();
		return sensors[index];
	}
	else if (sensors.size() > 0)
	{
		return sensors[0];
	}
	return string("");
}

bool setNextSensor(Sensor *sensorPtr, int offset)
{
	const char *currentPath = sensorPtr != nullptr && sensorPtr->Path != nullptr ? sensorPtr->Path : "";
	string nextSensor = findNextSensor(currentPath, offset);
	debug.printLine("Current path: %s", currentPath);
	debug.printLine("Next name: %s", nextSensor.c_str());
	if (nextSensor.size() > 0)
	{
		string path = basePath;
		path += nextSensor;
		path += "/";

		debug.printLine("Next path: %s", path.c_str());

		string *x = new string(path.c_str());

		sensorPtr->~Sensor();
		new (sensorPtr) Sensor(x->c_str()); // Mem leak
		debug.printLine("New sensor path: %s", sensorPtr->Path);
		return true;
	}
	return false;
}

vector<string> getModes()
{
	vector<string> modes;
	string buf;
	stringstream ss(sensor.getModes());
	while (ss >> buf)
		modes.push_back(buf);
	return modes;
}

int main(int argc, const char *argv[])
{
	display.setFont("Uni2-VGA16");

	vector<string> modes = getModes();

	const int tickRate = 200;

	uint64_t lastValue = 0;
	int dataCount = 0;
	int tickCount = 0;
	uint64_t lastTime = nowMs();

	while (!buttons.back())
	{
		static int m = 0;
		uint64_t start = nowMs();

		if (buttons.up())
		{
			display.clear();
			if (setNextSensor(&sensor, 1))
			{
				modes = getModes();
			}
			utils.msleep(250);
		}

		if (buttons.down())
		{
			display.clear();
			if (setNextSensor(&sensor, -1))
			{
				modes = getModes();
			}
			utils.msleep(250);
		}

		if (buttons.right())
		{
			m = (m + 1) % modes.size();
			const char *mode = modes[m].c_str();
			display.printClear("\nMode %i: %s\n", m + 1, mode);
			//debugString(mode);
			sensor.setMode(mode);
			utils.msleep(250);
		}
		if (buttons.left())
		{
			m = (m - 1 + modes.size()) % modes.size();
			const char *mode = modes[m].c_str();
			display.printClear("\nMode %i: %s\n", m + 1, mode);
			//debugString(mode);
			sensor.setMode(mode);
			utils.msleep(250);
		}

		uint64_t value = sensor.getBinData<uint64_t>();
		if (lastValue != value)
		{
			lastValue = value;
			dataCount++;
		}
		tickCount++;

		uint64_t elapsed = elapsedMs(lastTime);
		if (elapsed > 500)
		{
			string mode = sensor.getMode();
			vector<string>::iterator it = find(modes.begin(), modes.end(), mode);
			int index = distance(modes.begin(), it);

			int dataRate = roundf(dataCount * 1000.0f / elapsed);
			int tickRate = roundf(tickCount * 1000.0f / elapsed);

			display.clear();
			display.print("Sensor: %s\n", getSensorName(sensor.Path).c_str());
			display.print(" %s\n", sensor.getDriverName().c_str());
			display.print("Mode %i/%i: %s\n", index + 1, modes.size(), mode.c_str());
			display.print("Num: %i, Dec: %i\n", sensor.getNumValues(), sensor.getDecimals());
			display.print("Data: %s\n", sensor.getDebugValues().c_str());
			display.print("Data rate: %i Hz\n", dataRate);
			display.print("Tick rate: %i Hz\n", tickRate);

			lastTime = nowMs();
			dataCount = 0;
			tickCount = 0;
		}

		int sleepTime = (1000.0f / tickRate) - elapsedMs(start);
		if (sleepTime > 0)
		{
			utils.msleep(sleepTime);
		}
	}
	return 0;
}
