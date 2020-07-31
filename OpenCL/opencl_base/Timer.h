#pragma once
#include <chrono>
class Timer
{
	std::chrono::time_point<std::chrono::steady_clock> start_time = {};
	std::chrono::time_point<std::chrono::steady_clock> stop_time = {};

public:
	void start();
	void stop();
	std::chrono::milliseconds time_elapsed();

};