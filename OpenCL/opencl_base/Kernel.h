#pragma once
#include <CL/cl.h>
#include <fstream>

#include "PlatformAndDevice.h"
#include "Utils.h"

namespace Kernel
{
	cl_kernel compile_and_build_multiplication_kernel(cl_context& context, cl_device_id* gpu_ids)
	{
		cl_int err { 0 };
		// Read a single kernel file.
		// Should be pulled out into a function to read
		// multiple files if we need more than one kernel.
		std::ifstream kernel_file;
		size_t kernel_file_size { 0 };
		char* kernel_source { nullptr };
		Utils::read_kernel_code(kernel_file,
								"complex_multiply.cl",
								kernel_source,
								kernel_file_size);

		// Create a program from the kernel source
		cl_program program = clCreateProgramWithSource(
			context, 1, (const char**) &kernel_source,
			(const size_t*) &kernel_file_size, &err);

		// Build the program
		err = clBuildProgram(program, 1, gpu_ids, NULL, NULL, NULL);
		PlatformAndDevice::print_if_error(err, "clBuildProgram");;
		size_t buf_sz { 2048 };
		char* build_log = new char[buf_sz]();
		clGetProgramBuildInfo(program, gpu_ids[0], CL_PROGRAM_BUILD_LOG,
							  buf_sz, (void*) build_log, NULL);
		std::cout << build_log;

		// Create the OpenCL kernel
		cl_kernel kernel = clCreateKernel(program, "complex_multiply", &err);
		err = clReleaseProgram(program);
		delete[] kernel_source;
		delete[] build_log;
		return kernel;
	}

	void run_multiplication_kernel(cl_context& context,
								   cl_kernel& kernel,
								   cl_device_id* gpu_ids,
								   const cl_int LIST_SIZE,
								   const size_t global_item_size,
								   const size_t local_item_size,
								   cl_float2* complex_signal_frequencies,
								   cl_float2* complex_filter_frequencies,
								   cl_float2* complex_result)
	{
		cl_int err { 0 };
		cl_command_queue command_queue = clCreateCommandQueue(context, gpu_ids[0], 0, &err);

		// Create memory buffers on the device for each vector
		cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
										  LIST_SIZE * sizeof(cl_float2), NULL, &err);
		cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
										  LIST_SIZE * sizeof(cl_float2), NULL, &err);
		cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
										  LIST_SIZE * sizeof(cl_float2), NULL, &err);

		// Copy the lists A and B to their respective memory buffers
		err = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
								   LIST_SIZE * sizeof(cl_float2),
								   complex_signal_frequencies, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
								   LIST_SIZE * sizeof(cl_float2),
								   complex_filter_frequencies, 0, NULL, NULL);

		// Set the arguments of the kernel
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &a_mem_obj);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 0");
		err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &b_mem_obj);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 1");
		err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &c_mem_obj);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 2");

		// Add kernel to queue
		err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
									 &global_item_size, &local_item_size,
									 0, NULL, NULL);

		// Read the result memory buffer on the device to the local variable
		err = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
								  LIST_SIZE * sizeof(cl_float2),
								  (void*) complex_result, 0, NULL, NULL);

		err = clFlush(command_queue);
		err = clFinish(command_queue);
		err = clReleaseMemObject(a_mem_obj);
		err = clReleaseMemObject(b_mem_obj);
		err = clReleaseMemObject(c_mem_obj);
		err = clReleaseCommandQueue(command_queue);
	}

	cl_kernel compile_and_build_time_convolution_kernel(cl_context& context, cl_device_id* gpu_ids)
	{
		cl_int err { 0 };
		// Read a single kernel file.
		// Should be pulled out into a function to read
		// multiple files if we need more than one kernel.
		std::ifstream kernel_file;
		size_t kernel_file_size { 0 };
		char* kernel_source { nullptr };
		Utils::read_kernel_code(kernel_file,
								"time_convolution.cl",
								kernel_source,
								kernel_file_size);

		// Create a program from the kernel source
		cl_program program = clCreateProgramWithSource(
			context, 1, (const char**) &kernel_source,
			(const size_t*) &kernel_file_size, &err);

		// Build the program
		err = clBuildProgram(program, 1, gpu_ids, NULL, NULL, NULL);
		PlatformAndDevice::print_if_error(err, "clBuildProgram");;
		size_t buf_sz { 2048 };
		char* build_log = new char[buf_sz]();
		clGetProgramBuildInfo(program, gpu_ids[0], CL_PROGRAM_BUILD_LOG,
							  buf_sz, (void*) build_log, NULL);
		std::cout << build_log;

		// Create the OpenCL kernel
		cl_kernel kernel = clCreateKernel(program, "time_convolution", &err);
		err = clReleaseProgram(program);
		delete[] kernel_source;
		delete[] build_log;
		return kernel;
	}

	void run_time_convolution_kernel(cl_context& context,
									 cl_kernel& kernel,
									 cl_device_id* gpu_ids,
									 const cl_int LIST_SIZE,
									 const cl_int H_SIZE,
									 const size_t global_item_size,
									 const size_t local_item_size,
									 cl_float* input,
									 cl_float* h,
									 cl_float* output)
	{
		cl_int err { 0 };
		cl_command_queue command_queue = clCreateCommandQueue(context, gpu_ids[0], 0, &err);

		// Create memory buffers on the device for each vector 
		cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
										  LIST_SIZE * sizeof(cl_float), NULL, &err);
		cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
										  H_SIZE * sizeof(cl_float), NULL, &err);
		cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
										  LIST_SIZE * sizeof(cl_float), NULL, &err);

		// Copy the lists A and B to their respective memory buffers
		err = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
								   LIST_SIZE * sizeof(cl_float), input, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
								   H_SIZE * sizeof(cl_float), h, 0, NULL, NULL);

		// Set the arguments of the kernel
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &c_mem_obj);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 0");
		err = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*) &LIST_SIZE);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 1");
		err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &b_mem_obj);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 2");
		err = clSetKernelArg(kernel, 3, sizeof(cl_int), (void*) &H_SIZE);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 3");
		err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*) &a_mem_obj);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 4");
		err = clSetKernelArg(kernel, 5, sizeof(cl_int), (void*) &LIST_SIZE);
		PlatformAndDevice::print_if_error(err, "clSetKernelArg 5");

		// Add kernel to queue
		err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
									 &global_item_size, &local_item_size,
									 0, NULL, NULL);

		// Read the result memory buffer on the device to the local variable
		err = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
								  LIST_SIZE * sizeof(cl_float),
								  (void*) output, 0, NULL, NULL);

		err = clFlush(command_queue);
		err = clFinish(command_queue);
		err = clReleaseMemObject(a_mem_obj);
		err = clReleaseMemObject(b_mem_obj);
		err = clReleaseMemObject(c_mem_obj);
		err = clReleaseCommandQueue(command_queue);
	}
}