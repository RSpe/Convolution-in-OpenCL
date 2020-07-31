#pragma once
#include <CL/cl.h>
#include <vexcl/vexcl.hpp>
#include <fstream>

#include "PlatformAndDevice.h"
#include "kernel.h"
#include "Utils.h"
#include "FileIO.h"

// will only handle one chunk
void frequency_convolution(std::string input_filepath, std::string output_filepath,
	const int CHUNK_SIZE, const int TAP_SIZE)
{

	// set up all file params
	FileIO::FileContext file_context = *new FileIO::FileContext();
	file_context.input_filepath = input_filepath.c_str();
	file_context.output_filepath = output_filepath.c_str();
	file_context.CHUNK_SIZE = CHUNK_SIZE;
	file_context.TAP_SIZE = TAP_SIZE;
	FileIO::set_vector_sizes(file_context);
	FileIO::map_file(file_context);
	FileIO::set_csv_writer(file_context);
	bool is_mapped = FileIO::file_context_is_mapped(file_context);
	std::string status = is_mapped ? "succeed" : "failed";
	std::cout << "file mapped " << status << std::endl;

	// get context for Vex
	vex::Context ctx(vex::Filter::GPU);
	if (!ctx) throw std::runtime_error("No devices available.");
	// Print out list of selected devices:
	std::cout << ctx << std::endl;

	// Get the platofrm and device info for multiplication kernel
	cl_platform_id platform_ids[8]{ 0 };
	cl_uint num_platforms{ 1 }; // limted to one currently
	cl_device_id gpu_ids[8]{ 0 };
	cl_uint num_devices{ 1 }; // limted to one currently
	char* vendor = new char[1024]();
	char* name = new char[1024]();
	PlatformAndDevice::get_platform_info(platform_ids, num_platforms, vendor, name);
	PlatformAndDevice::get_device_info(platform_ids, gpu_ids, num_devices);
	std::cout
		<< "Vendor: " << vendor << "\n"
		<< "Platform: " << name << "\n"
		<< "GPU ID: " << gpu_ids[0] << std::endl;
	delete[] vendor;
	delete[] name;

	// Create an OpenCL context for multiplication kernel and build it.
	cl_int err{ 0 };
	cl_context context = clCreateContext(NULL, 1, gpu_ids, NULL, NULL, &err);
	cl_kernel kernel = Kernel::compile_and_build_multiplication_kernel(context, gpu_ids);

	// Sizes
	const cl_int LIST_SIZE = static_cast<cl_int>(CHUNK_SIZE);
	const cl_int H_SIZE = static_cast<cl_int>(TAP_SIZE);

	// setup fft functions
	vex::FFT<float, cl_float2> fft(ctx, LIST_SIZE);
	vex::FFT<cl_float2, float> ifft(ctx, LIST_SIZE, vex::fft::inverse);

	// Delcare inital vectors
	vex::vector<cl_float> input(ctx, LIST_SIZE);
	vex::vector<cl_float> h(ctx, LIST_SIZE);

	// read first chunk
	FileIO::start_read_no_pad(file_context);
	Utils::copy_vector(file_context.signal, input);
	Utils::copy_vector(file_context.fir, h);

	// Declare vectors for fft data and output
	vex::vector<cl_float2> input_intermediate(ctx, LIST_SIZE);
	vex::vector<cl_float2> h_intermediate(ctx, LIST_SIZE);
	bool chunks_left = true;
	while (chunks_left)
	{
		// run fft on data
		input_intermediate = fft(input);
		h_intermediate = fft(h);

		// Create arrays for Kernel execution function
		cl_float2* complex_signal_frequencies = new cl_float2[LIST_SIZE]();
		cl_float2* complex_filter_frequencies = new cl_float2[LIST_SIZE]();
		cl_float2* complex_result = new cl_float2[LIST_SIZE]();

		// copy over data from vectors to arrays
		Utils::vector_to_array(input_intermediate, complex_signal_frequencies);
		Utils::vector_to_array(h_intermediate, complex_filter_frequencies);

		size_t global_item_size{ static_cast<size_t>(LIST_SIZE) };
		size_t local_item_size{ 64 };

		Kernel::run_multiplication_kernel(context,
			kernel,
			gpu_ids, LIST_SIZE,
			global_item_size,
			local_item_size,
			complex_signal_frequencies,
			complex_filter_frequencies,
			complex_result);

		// Convert 
		vex::vector<cl_float2> c_vector(ctx, LIST_SIZE);
		Utils::array_to_vector(c_vector, complex_result);
		// Clean up the arg arrays
		delete[] complex_signal_frequencies;
		delete[] complex_filter_frequencies;
		delete[] complex_result;

		vex::vector<cl_float> output(ctx, LIST_SIZE);
		output = ifft(c_vector);
		Utils::copy_vector(output, file_context.out_signal);
		FileIO::write_chunk(file_context);
		chunks_left = FileIO::get_next_chunk(file_context);
	}

	// Clean up
	err = clReleaseContext(context);
	err = clReleaseKernel(kernel);
}