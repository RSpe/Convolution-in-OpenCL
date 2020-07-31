#pragma once
#include "Timer.h"

void Timer::start()
{
	start_time = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	stop_time = std::chrono::high_resolution_clock::now();
}

std::chrono::milliseconds Timer::time_elapsed()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time);
}