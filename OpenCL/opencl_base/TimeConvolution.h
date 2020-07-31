#pragma once
#include <CL/cl.h>
#include <fstream>
#include <thread>
#include <cmath>

#include "PlatformAndDevice.h"
#include "Utils.h"
#include "Kernel.h"
#include "FileIO.h"

void time_convolution(std::string input_filepath, std::string output_filepath,
					  const int CHUNK_SIZE, const int TAP_SIZE)
{
	// Create the two input vectors
	const cl_int LIST_SIZE = static_cast<cl_int>(CHUNK_SIZE);
	const cl_int H_SIZE = static_cast<cl_int>(TAP_SIZE);

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


	cl_float* input = new cl_float[LIST_SIZE]();
	cl_float* h = new cl_float[H_SIZE]();
	cl_float* output = new cl_float[LIST_SIZE]();

	// read first chunk
	FileIO::start_read(file_context);
	Utils::copy_vector(file_context.tap_coef, h);

	// Get the platofrm and device info
	// could be generalised to more than one platform, but my pc
	// only had one.
	cl_platform_id platform_ids[8] { 0 };
	cl_uint num_platforms { 1 };
	cl_device_id gpu_ids[8] { 0 };
	cl_uint num_devices { 1 };
	char* vendor = new char[1024]();
	char* name = new char[1024]();
	PlatformAndDevice::get_platform_info(platform_ids, num_platforms, vendor, name);
	PlatformAndDevice::get_device_info(platform_ids, gpu_ids, num_devices);
	std::cout
		<< "Vendor: " << vendor << "\n"
		<< "Platform: " << name << "\n"
		<< "GPU ID: " << gpu_ids[0] << std::endl;

	cl_int err { 0 };
	cl_context context = clCreateContext(NULL, 1, gpu_ids, NULL, NULL, &err);
	cl_kernel kernel = Kernel::compile_and_build_time_convolution_kernel(context, gpu_ids);

	// Queue params
	size_t global_item_size { static_cast<size_t>(LIST_SIZE) };
	size_t local_item_size { static_cast<size_t>(pow(2,6)) };
	// copy first chunk
	Utils::copy_vector(file_context.signalPadded, input);

	bool chunks_left = true;
	while (chunks_left)
	{
		Kernel::run_time_convolution_kernel(context, kernel, gpu_ids,
											LIST_SIZE, H_SIZE, global_item_size,
											local_item_size,
											input, h, output);
		// copy back data.
		Utils::array_to_vector(file_context.out_signal, output);

		std::thread writer(&CSV_Writer::writeData,
						   file_context.csv_writer,
						   file_context.out_signal);

		chunks_left = FileIO::get_next_chunk(file_context);
		Utils::copy_vector(file_context.signal, input);

		writer.join();
	}
	// Clean up
	err = clReleaseContext(context);
	err = clReleaseKernel(kernel);
}