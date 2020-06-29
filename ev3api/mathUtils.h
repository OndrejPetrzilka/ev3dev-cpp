#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

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
