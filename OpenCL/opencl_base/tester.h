#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iostream>

#define T_FILE "timingResults.csv"

class Tester {

public:
	Tester();
	~Tester();

	void startTiming(std::string testName);
	void endTiming();
	void writeToFile();

private:
	
	std::vector<std::string> testNames;
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point endTime;

	std::vector<long long> testTimes;

	long long calcTimeTaken();

};