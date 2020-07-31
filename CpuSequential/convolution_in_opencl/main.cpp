#include <fstream>
#include <string>

#include "cpu_sequential/cpu_sequential.h"
#include "cpu_sequential/generate_data_filter.h"

#include <chrono>

using namespace std::chrono;

int main(void)
{
	cpu_sequential cpu_sequential;
	auto start = high_resolution_clock::now();
	cpu_sequential.overlap_save();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << duration.count() << " milliseconds" << std::endl;
}