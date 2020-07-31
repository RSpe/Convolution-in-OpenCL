__kernel void time_convolution(__global float* output, const int OUTPUT_LENGTH,
							   __global float* impulse_response, const int RESPONSE_LEGNTH,
							   __global float* input, const int INPUT_LENGTH)
	/*
	Input is a padded array (with 0s).
	Padded as borders/boundaries cannot be computed as normal.
	Kernel is the array of impulse responses.
	*/
{
	int i = get_global_id(0) + RESPONSE_LEGNTH;
	for (int j = 0; j < RESPONSE_LEGNTH; ++j)
	{
		output[i - RESPONSE_LEGNTH] += input[i - j] * impulse_response[j];
	}
}