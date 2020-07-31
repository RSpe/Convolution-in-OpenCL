#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <fftw3.h>

#include "generate_data_filter.h"

class cpu_sequential
{
public:

	int max_filter_size = 512;

	cpu_sequential();
	~cpu_sequential();

	void overlap_save();
	fftw_complex* fft_filter(std::vector<double> input_vector, int block_length);
	double* ifft_filter(fftw_complex* complex_input, int block_length);
	fftw_complex* multiply(fftw_complex* dft1, fftw_complex* dft2, int n);
	//void fftw();

	std::string filename = "test_134217728.csv";

private:

	std::string input_types;

	std::vector<double> time_data;
	std::vector<double> signal_data;
	std::vector<double> filter_coefficient_data;
	std::vector<double> signal_freq;

	std::vector<double> output_array;

	//double* input = (double*)fftw_malloc(sizeof(float) * data_length);
	//fftw_complex* output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * data_length);

	fftw_plan fft;
	fftw_plan ifft;

};