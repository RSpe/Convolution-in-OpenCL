#include "generate_data_filter.h"

const std::vector<double> generate_data_filter::generate(int seed, int size)
{
	std::vector<double> data_filter;

	srand(seed);

	for (int i = 0; i < size; i++) 
	{
		double single_precision = fmod(static_cast<double>(rand()), 10 + 1);
		data_filter.push_back(single_precision);
	}

	return data_filter;
}