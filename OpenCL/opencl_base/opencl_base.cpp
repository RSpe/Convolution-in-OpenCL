// std lib
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <fstream>

// 3rd party
#include <CL/cl.h>
#include <fftw3.h>

// 1st party
#include "TimeConvolution.h"
#include "FrequencyConvolution.h"
#include "OverlapSave.h"
#include "Timer.h"

template <typename T>
void get_user_option(std::string message, std::string& option, T& variable)
{
	std::cout << message << " {"<< variable << "} Y/N?: ";
	std::cin >> option;
	std::cout << std::endl;

	if (option == "N" || option == "n")
	{
		std::cout << "please enter : ";
		std::cin >> variable;
		std::cout << std::endl;
	}
}


int main(void)
{
	Timer timer = Timer();
	std::string option {};
	std::string input_filepath { "..\\..\\Data\\small_test.csv" };
	std::string output_filepath { "output.csv" };
	size_t CHUNK_SIZE { 6 };
	size_t TAP_SIZE { 3 };
	size_t FILE_LENGTH = Utils::get_file_length(input_filepath) - 1;

	std::cout << FILE_LENGTH << std::endl;

	get_user_option("use default chunk size", option, CHUNK_SIZE);
	get_user_option("use default tap size", option, TAP_SIZE);
	get_user_option("use default input filepath", option, input_filepath);
	get_user_option("use default output filepath", option, output_filepath);
	// Will only do one chunk as it doesnt handle overlaps etc
	//timer.start();
	//frequency_convolution(input_filepath, output_filepath, CHUNK_SIZE, TAP_SIZE);
	//timer.stop();
	//std::cout << "Frequency convolution time taken "<< timer.time_elapsed().count() << "ms\n";

	timer.start();
	time_convolution(input_filepath, output_filepath, CHUNK_SIZE, TAP_SIZE);
	timer.stop();
	std::cout << "Time convolution time taken " << timer.time_elapsed().count() << "ms\n";

	timer.start();
	overlap_save(input_filepath, output_filepath, CHUNK_SIZE, TAP_SIZE, FILE_LENGTH);
	timer.stop();
	std::cout << "Overlap save time taken " << timer.time_elapsed().count() << "ms\n";
}