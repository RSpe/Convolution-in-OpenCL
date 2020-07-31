#pragma once
#include <CL/cl.h>
#include <vexcl/vexcl.hpp>
#include <fstream>
#include <cmath>

#include "PlatformAndDevice.h"
#include "kernel.h"
#include "Utils.h"
#include "FileIO.h"

void overlap_save(std::string input_filepath, std::string output_filepath, const int CHUNK_SIZE, const int TAP_SIZE, const int FILE_SIZE)
{

    // set up all file params
    FileIO::FileContext file_context = *new FileIO::FileContext();
    file_context.input_filepath = input_filepath.c_str();
    file_context.output_filepath = output_filepath.c_str();
    file_context.CHUNK_SIZE = CHUNK_SIZE;
    file_context.TAP_SIZE = TAP_SIZE - 1;
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

    FileIO::start_read(file_context);

    // Get the platofrm and device info for multiplication kernel
    // could be generalised to more than one platform, but my pc
    // only had one.
    cl_platform_id platform_ids[8]{ 0 };
    cl_uint num_platforms{ 1 };
    cl_device_id gpu_ids[8]{ 0 };
    cl_uint num_devices{ 1 };
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

    // Create an OpenCL context for multiplication kernel
    cl_int err{ 0 };
    cl_context context = clCreateContext(NULL, 1, gpu_ids, NULL, NULL, &err);
    cl_kernel kernel = Kernel::compile_and_build_multiplication_kernel(context, gpu_ids);

    // Sizes
    const cl_int LIST_SIZE = static_cast<cl_int>(CHUNK_SIZE);
    const cl_int H_SIZE = static_cast<cl_int>(TAP_SIZE);

    int overlap = H_SIZE - 1; // The amount of overlap each block of data will need when performing each fft convolution 
    int n = LIST_SIZE; // Length of block
    int step_size = n - overlap; // How much new data is contained in the block
    int position = 0; // Current index value in the signal data

    // setup fft functions 
    vex::FFT<float, cl_float2> fft(ctx, n);
    vex::FFT<cl_float2, float> ifft(ctx, n, vex::fft::inverse);

    // Delcare inital vectors
    //vex::vector<cl_float> input(ctx, LIST_SIZE);
    vex::vector<cl_float> h(ctx, n);

    // read first fir filter
    Utils::copy_vector(file_context.fir, h);

    // Declare vectors for fft data and output
    vex::vector<cl_float2> filter_fft(ctx, n);

    // Run fft on fir filter
    filter_fft = fft(h);

    // Creating array to store dft data (in this case fir) because kernel can not handle vectors
    cl_float2* complex_filter_frequencies = new cl_float2[n]();

    // Any vector to array is so data can be passed to the kernel, array to vector is for easy data manipulation in the host code
    Utils::vector_to_array(filter_fft, complex_filter_frequencies);

    // Create output vector, remove first element being data size
    std::vector<cl_float> output(ctx, LIST_SIZE);
    output.erase(output.begin());

    size_t global_item_size{ static_cast<size_t>(LIST_SIZE) };
    size_t local_item_size = n;

    // Overlap save performed on blocks of data
    while ((position + n) <= FILE_SIZE)
    {
        std::vector<cl_float> output;

        vex::vector<cl_float> temp_input(ctx, n);

        // Depending on the position of the start of the next block (position = index of the complete signal data),
        // pad the beginning block with 0s and the end block, blocks in between are padded with the previous blocks input
        if (position == 0)
        {
            Utils::copy_vector(file_context.signalPadded, temp_input);
        }

        else
        {
            FileIO::get_next_padded_chunk(file_context);
            Utils::copy_vector(file_context.signalPadded, temp_input);
        }

        // Intialise vex vectors to hold dft and complex data
        vex::vector<cl_float2> dft(ctx, n);
        vex::vector<cl_float2> multiply(ctx, n);
        vex::vector<cl_float> idft(ctx, n);

        dft = fft(temp_input);

        cl_float2* complex_signal_frequencies = new cl_float2[n]();
        cl_float2* complex_result = new cl_float2[n]();

        Utils::vector_to_array(dft, complex_signal_frequencies);

        // OpenCl kernel to multiple the complex arrays (fir filter multiplied by the current chunk)
        Kernel::run_multiplication_kernel(context, kernel, gpu_ids, n,
            global_item_size,
            local_item_size,
            complex_signal_frequencies,
            complex_filter_frequencies,
            complex_result);

        vex::vector<cl_float2> c_vector(ctx, n);
        Utils::array_to_vector(c_vector, complex_result);

        idft = ifft(c_vector);

        Utils::array_to_vector(c_vector, complex_result);

        // Store the result of overlap save on the current chunk, remove the values used for padding as they are not needed for the final result
        for (int x = overlap; x < n; x++)
        {
            output.push_back(idft[x]);
        }

        file_context.out_signal.resize(step_size);
        Utils::copy_vector(output, file_context.out_signal);
        FileIO::write_chunk(file_context);

        // Free arrays so they can be used again in the next overlap save cycle
        delete[] complex_signal_frequencies;
        delete[] complex_result;

        // Increment the position by the step_size so the next chunk will pick up the overlapped data
        position += step_size;
    }

    // Free complex_filter_frequencies after it is no longer used
    delete[] complex_filter_frequencies;
    err = clReleaseKernel(kernel);
}