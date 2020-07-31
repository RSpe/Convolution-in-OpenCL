#include "cpu_sequential.h"

/* Reads in generated signal data and performs overlap save sequentially (frequency domain) */

cpu_sequential::cpu_sequential()
{
    // Do not need random generation, have signal CSV data
    // generate_filter_coefficients = generate_data_filter::generate(1, data_length);
    // generate_signal_data = generate_data_filter::generate(2, data_length);

    std::ifstream infile(filename);

    int current_filter = 0;

    std::string temp_time, temp_coefficient, temp_signal;

    std::getline(infile, input_types); // First line coloumn types

    std::string line;

    // Reads the whole signal data and filter data into memory and adds them to their corresponding vectors
    while (std::getline(infile, line))
    {
        std::stringstream stream(line);

        std::getline(stream, temp_time, ',');
        std::getline(stream, temp_coefficient, ',');
        std::getline(stream, temp_signal, '\n');

        signal_data.push_back(std::stod(temp_signal));

        if (current_filter < max_filter_size)
        {
            filter_coefficient_data.push_back(std::stod(temp_coefficient));
            current_filter += 1;
        }
    }
}

cpu_sequential::~cpu_sequential()
{

}

void cpu_sequential::overlap_save()
{

    int length_data = signal_data.size(); // Total amount of signal data to process

    int m = filter_coefficient_data.size(); // Used to find overlap
    int overlap = m - 1; // The amount of overlap each block of data will need when performing each fft convolution 
    int n = 4096; // Length of block
    int step_size = n - overlap; // How much new data is contained in the block
    int position = 0; // Current index value in the signal data

    // Pad the filter to be the same length as the block, important for complex multiplcation step
    for (int i = m; i < n; ++i)
    {
        filter_coefficient_data.push_back(0);
    }
    
    // Precompute the DFT of the filter
    fftw_complex* fir_fft = fft_filter(filter_coefficient_data, n);

    // Pad the start and the end of the signal data with 0s the same length as the overlap
    for (int i = 0; i < overlap; ++i)
    {
        signal_data.insert(signal_data.begin(), 0);
        signal_data.insert(signal_data.end(), 0);
    }

    // Overlap save block computation
    while ((position + n) < length_data)
    {
        std::vector<double> temp_signal(&signal_data[position], &signal_data[position + n]); // Set block to process
        fftw_complex* dft = fft_filter(temp_signal, n); // Perform fft on block
        fftw_complex* dft_h = multiply(dft, fir_fft, n); // Perform complex multiplication on block with precomputed finitie impulse response filter dft
        double* idft = ifft_filter(dft_h, n); // Perform inverse dft on the multiplication result

        // Index 0 to index step_size are all the overlapped values in the block and they are not part of the final result so are ignored
        for (int j = step_size; j < n; j++)
        {
            signal_freq.push_back(idft[j]);
        }
        position += step_size; // Increment position to find the start of the next block (accounting for overlap)
    }

    // Print the final frequency output
    //for (int k = 0; k < signal_freq.size(); k++)
    //{
    //    std::cout << signal_freq[k] << std::endl;
    //}
}

// Performs complex multiplcation for frequency convolution
fftw_complex* cpu_sequential::multiply(fftw_complex* dft1, fftw_complex* dft2, int n)
{

    for (int i = 0; i < n; i++)
    {
        double x1 = dft1[i][0];
        double y1 = dft1[i][1];
        double x2 = dft2[i][0];
        double y2 = dft2[i][1];

        dft1[i][0] = (x1 * x2) - (y1 * y2);
        dft1[i][1] = (x1 * y2) + (y1 * x2);
    }

    return dft1;
}

// Performs dft
fftw_complex* cpu_sequential::fft_filter(std::vector<double> input_vector, int block_size)
{
    double* input_array = &input_vector[0];

    fftw_complex* complex = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * block_size);

    // Real to complex dft
    fft = fftw_plan_dft_r2c_1d(block_size, input_array, complex, FFTW_ESTIMATE); // Finds optimal FFT by calc a few samples, more overhead.
    fftw_execute(fft);

    fftw_destroy_plan(fft);
    fftw_cleanup();

    return complex;
}

// Performs inverse dft
double* cpu_sequential::ifft_filter(fftw_complex* complex_input, int block_size)
{
    double* output = (double*)fftw_malloc(sizeof(float) * block_size * 2);

    // Complex to real idft
    ifft = fftw_plan_dft_c2r_1d(block_size, complex_input, output, FFTW_ESTIMATE); // Finds optimal FFT by calc a few samples, more overhead.
    fftw_execute(ifft);

    fftw_destroy_plan(ifft);
    fftw_cleanup();

    // Divide each output value by the block size to get the exact inverse
    for (int i = 0; i < block_size; ++i)
    {
        output[i] /= block_size;
    }

    return output;
}