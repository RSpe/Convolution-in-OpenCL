#include "tester.h"

Tester::Tester(){

}

Tester::~Tester() {

}

void Tester::startTiming(std::string testName) {
	this->testNames.push_back(testName);
	this->startTime = std::chrono::high_resolution_clock::now();

}

void Tester::endTiming() {
	this->endTime = std::chrono::high_resolution_clock::now();

	this->testTimes.push_back(calcTimeTaken());
}

long long Tester::calcTimeTaken() {
	return std::chrono::duration_cast<std::chrono::microseconds>(this->endTime - this->startTime).count();
}

void Tester::writeToFile() {

	std::ofstream outFile;

	outFile.open(T_FILE, std::ofstream::out | std::ofstream::trunc);

	if (outFile.is_open()) {
		std::cout << "Congrats you opened the file" << std::endl;
	}
	else {
		std::cout << "Failed to open the file" << std::endl;
	}

	//Add the headers to the file.
	outFile << "Test_Name, Time"<< std::endl;

	for (int i = 0; i < (testTimes.size()); i++) {
		outFile << testNames.at(i) << ',' << testTimes.at(i) << std::endl;

	}

	outFile.close();
}